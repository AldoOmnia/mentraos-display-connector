import dotenv from 'dotenv';
dotenv.config();

import express from 'express';
import { MentraOSApp, LayoutType, ViewType } from '@mentra/sdk';
import SerialDisplayController from './display/serialDisplayController.js';
import DashboardController from './dashboard/dashboardController.js';
import VoiceCommandProcessor from './voice/voiceCommandProcessor.js';
import WebSearchService from './search/webSearchService.js';
import Logger from './utils/logger.js';

const logger = new Logger('main');
const app = express();
const port = process.env.PORT || 3000;

// Initialize services
const displayController = new SerialDisplayController({
  port: process.env.SERIAL_PORT,
  baudRate: parseInt(process.env.SERIAL_BAUD_RATE) || 9600,
  width: parseInt(process.env.DISPLAY_WIDTH) || 128,
  height: parseInt(process.env.DISPLAY_HEIGHT) || 56
});

const dashboardController = new DashboardController();
const webSearchService = new WebSearchService();
const voiceCommandProcessor = new VoiceCommandProcessor(displayController, dashboardController);

// Create the MentraOS app
const mentraApp = new MentraOSApp({
  packageName: process.env.PACKAGE_NAME,
  apiKey: process.env.MENTRAOS_API_KEY,
});

// Initialize the app
async function init() {
  try {
    // Initialize the display controller
    await displayController.initialize();
    logger.info('Display controller initialized');

    // Set up the express endpoint that MentraOS will call
    app.post('/webhook', express.json(), async (req, res) => {
      const { sessionId, userId, event } = req.body;
      
      logger.info(`Received webhook: sessionId=${sessionId}, userId=${userId}, event=${event}`);
      res.status(200).send('OK');
      
      try {
        // Start a new MentraOS session
        await mentraApp.handleWebhook(req.body);
      } catch (error) {
        logger.error(`Error handling webhook: ${error.message}`);
      }
    });

    // Add search API endpoint
    app.post('/api/search', express.json(), async (req, res) => {
      try {
        const { query } = req.body;
        
        if (!query) {
          return res.status(400).json({ 
            success: false, 
            error: 'Query parameter is required' 
          });
        }
        
        logger.info(`Web search API request: "${query}"`);
        const searchResults = await webSearchService.searchWeb(query);
        
        // Also display on OLED if connected
        if (displayController.isConnected && searchResults.success) {
          const displayText = webSearchService.formatResultsForDisplay(searchResults);
          await displayController.sendCommand('clear');
          await displayController.sendCommand(`multiline:${displayText.replace(/\n/g, '\\n')}`);
        }
        
        return res.json(searchResults);
      } catch (error) {
        logger.error(`Search API error: ${error.message}`);
        return res.status(500).json({ 
          success: false, 
          error: error.message 
        });
      }
    });

    // Add a simple status endpoint
    app.get('/status', (req, res) => {
      res.json({
        status: 'running',
        displayConnected: displayController.isConnected,
        searchEnabled: !!process.env.PERPLEXITY_API_KEY,
        version: '0.2.0',
        timestamp: new Date().toISOString()
      });
    });

    // Start the express server
    app.listen(port, () => {
      logger.info(`Server listening on port ${port}`);
    });
    
    // Handle MentraOS session start
    mentraApp.onSession(async (session, sessionId, userId) => {
      logger.info(`New session started: ${sessionId} for user ${userId}`);
      
      // Display welcome message on the OLED
      await displayController.displayText('MentraOS Connected!', 10, 10);
      
      // Initialize dashboard
      await dashboardController.initializeDashboard(session);
      
      // Show welcome message in glasses
      await session.layouts.showTextWall('OLED Display App Ready', { durationMs: 5000 });
      
      // Set up event handlers for this session
      setupEventHandlers(session);
    });
    
    logger.info('Application initialized successfully');
    
  } catch (error) {
    logger.error(`Initialization error: ${error.message}`);
    process.exit(1);
  }
}

// Set up event handlers for a session
function setupEventHandlers(session) {
  // Handle transcription events (voice commands)
  session.events.onTranscription(async (data) => {
    const { text, isFinal } = data;
    
    if (isFinal) {
      logger.info(`Final transcription received: "${text}"`);
      
      try {
        // Process voice commands using the voice command processor
        const result = await voiceCommandProcessor.processTranscription(text, session);
        
        if (!result) {
          // No command matched, provide feedback
          logger.info('No command recognized in transcription');
          await session.layouts.showTextWall('No command recognized. Say "help" for commands.', { durationMs: 3000 });
        }
      } catch (error) {
        logger.error('Error processing voice command', error);
        await session.layouts.showTextWall(`Error: ${error.message}`, { durationMs: 3000 });
      }
    }
  });
  
  // Handle button press events
  session.events.onButtonPress(async (data) => {
    const { button } = data;
    logger.info(`Button pressed: ${button}`);
    
    try {
      switch(button) {
        case 'SELECT':
          await displayController.sendCommand('clear');
          await displayController.displayText('Button: SELECT', 10, 10);
          await dashboardController.updateDisplayContent(session, 'SELECT Button');
          await session.layouts.showTextWall('SELECT Button Pressed', { durationMs: 2000 });
          break;
          
        case 'BACK':
          await displayController.sendCommand('show_index');
          await dashboardController.updateDisplayContent(session, 'Index Screen');
          await session.layouts.showTextWall('BACK Button Pressed', { durationMs: 2000 });
          break;
          
        case 'UP':
          await displayController.sendCommand('directions');
          await dashboardController.updateDisplayContent(session, 'Directions');
          await session.layouts.showTextWall('UP Button Pressed', { durationMs: 2000 });
          break;
          
        case 'DOWN':
          await displayController.sendCommand('weather');
          await dashboardController.updateDisplayContent(session, 'Weather');
          await session.layouts.showTextWall('DOWN Button Pressed', { durationMs: 2000 });
          break;
      }
    } catch (error) {
      logger.error(`Error handling button press: ${error.message}`);
    }
  });
  
  // Handle capabilities events
  session.events.onCapabilities(async (capabilities) => {
    logger.info('Received capabilities', capabilities);
    
    // Update dashboard with device info
    try {
      const currentContent = await session.dashboard.getContent();
      if (currentContent && currentContent.expanded && currentContent.expanded.sections) {
        // Add a new section for device info if not already present
        if (currentContent.expanded.sections.length === 1) {
          currentContent.expanded.sections.push({
            title: 'Device Info',
            items: [
              { label: 'Has Display', value: capabilities.hasDisplay ? 'Yes' : 'No' },
              { label: 'Has Microphone', value: capabilities.hasMicrophone ? 'Yes' : 'No' }
            ]
          });
          
          await session.dashboard.updateContent({
            expanded: currentContent.expanded
          });
        }
      }
    } catch (error) {
      logger.error('Error updating dashboard with capabilities', error);
    }
  });
  
  // Handle session end
  session.events.onEnd(async () => {
    logger.info('Session ended');
    try {
      // Display disconnection message
      await displayController.displayText('MentraOS Disconnected', 10, 20);
      
      // Clear dashboard content
      await dashboardController.clearDashboard(session);
    } catch (error) {
      logger.error('Error handling session end', error);
    }
  });
}

// Initialize the application
init().catch(err => {
  logger.error(`Fatal error: ${err.message}`);
  process.exit(1);
});
