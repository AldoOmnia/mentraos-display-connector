import Logger from '../utils/logger.js';
import WebSearchService from '../search/webSearchService.js';

/**
 * Handles voice command processing for the MentraOS app
 */
class VoiceCommandProcessor {
  constructor(displayController, dashboardController) {
    this.displayController = displayController;
    this.dashboardController = dashboardController;
    this.logger = new Logger('VoiceCommandProcessor');
    this.webSearchService = new WebSearchService();
    
    // Define command patterns
    this.commandPatterns = [
      {
        pattern: /(?:show|display) text\s+(.+)/i,
        handler: this.handleShowText.bind(this),
        description: 'Show text on display'
      },
      {
        pattern: /(?:show|display) temperature/i,
        handler: this.handleShowTemperature.bind(this),
        description: 'Show temperature with degree symbol'
      },
      {
        pattern: /(?:show|display|set) time/i,
        handler: this.handleShowTime.bind(this),
        description: 'Show current time'
      },
      {
        pattern: /(?:show|display) weather/i,
        handler: this.handleWeatherCommand.bind(this),
        description: 'Show weather demo'
      },
      {
        pattern: /(?:show|display) forecast/i,
        handler: this.handleForecastCommand.bind(this),
        description: 'Show weather forecast with symbols'
      },
      {
        pattern: /(?:show|display) logo/i,
        handler: this.handleShowLogo.bind(this),
        description: 'Show logo'
      },
      {
        pattern: /(?:show|display) directions/i,
        handler: this.handleDirections.bind(this),
        description: 'Show direction arrows'
      },
      {
        pattern: /(?:clear|clean) (?:screen|display)/i,
        handler: this.handleClearScreen.bind(this),
        description: 'Clear screen'
      },
      {
        pattern: /(?:help|show help|commands|show commands)/i,
        handler: this.handleHelp.bind(this),
        description: 'Show help'
      },
      // Add web search commands
      {
        pattern: /(?:search|lookup|find|google|web search)(?: for)? (.+)/i,
        handler: this.handleWebSearch.bind(this),
        description: 'Search the web'
      },
      {
        pattern: /(?:what is|who is|tell me about|info on) (.+)/i,
        handler: this.handleWebSearch.bind(this),
        description: 'Get information about a topic'
      }
    ];
  }
  
  /**
   * Process a transcription to identify and execute commands
   * @param {string} text - The transcription text
   * @param {Object} session - MentraOS session
   * @returns {Object} - Result with matched command or null
   */
  async processTranscription(text, session) {
    this.logger.info(`Processing transcription: "${text}"`);
    
    for (const command of this.commandPatterns) {
      const match = text.match(command.pattern);
      
      if (match) {
        this.logger.info(`Matched command: ${command.description}`);
        try {
          // Execute the handler with the matched groups and session
          const result = await command.handler(match, session);
          return {
            command: command.description,
            result: result
          };
        } catch (error) {
          this.logger.error(`Error executing command: ${error.message}`);
          return {
            command: command.description,
            error: error.message
          };
        }
      }
    }
    
    this.logger.info('No command matched');
    return null;
  }
  
  /**
   * Handle "show text" command
   * @param {Array} match - Regex match groups
   * @param {Object} session - MentraOS session
   */
  async handleShowText(match, session) {
    const text = match[1] || '';
    
    // Display on OLED
    await this.displayController.displayText(text);
    
    // Update dashboard
    await this.dashboardController.updateDisplayContent(session, text);
    
    // Show in glasses
    await session.layouts.showTextWall(`Displaying: ${text}`, { durationMs: 3000 });
    
    return { text };
  }
  
  /**
   * Handle temperature display
   * @param {Array} match - Regex match groups
   * @param {Object} session - MentraOS session
   */
  async handleShowTemperature(match, session) {
    // Example temperature with degree symbol
    const temp = "72°F";
    
    // Send command with special char handling
    await this.displayController.sendCommand(`textxy:30,20,Temp: 72\\deg F`);
    
    // Update dashboard
    await this.dashboardController.updateDisplayContent(session, `Temp: ${temp}`);
    
    // Show in glasses
    await session.layouts.showTextWall(`Temperature: ${temp}`, { durationMs: 3000 });
    
    return { temperature: temp };
  }
  
  /**
   * Handle show time command
   * @param {Array} match - Regex match groups
   * @param {Object} session - MentraOS session
   */
  async handleShowTime(match, session) {
    const now = new Date();
    const timeStr = now.toLocaleTimeString();
    
    // Display time on OLED
    await this.displayController.sendCommand(`text:${timeStr}`);
    
    // Update dashboard
    await this.dashboardController.updateDisplayContent(session, timeStr);
    
    // Show in glasses
    await session.layouts.showTextWall(`Time: ${timeStr}`, { durationMs: 3000 });
    
    return { time: timeStr };
  }
  
  /**
   * Handle weather display command
   * @param {Array} match - Regex match groups
   * @param {Object} session - MentraOS session
   */
  async handleWeatherCommand(match, session) {
    // Use the weather command
    await this.displayController.sendCommand('weather');
    
    // Update dashboard
    await this.dashboardController.updateDisplayContent(session, 'Weather demo');
    
    // Show in glasses
    await session.layouts.showTextWall('Weather demo displayed', { durationMs: 3000 });
    
    return { success: true };
  }
  
  /**
   * Handle forecast display with special symbols
   * @param {Array} match - Regex match groups
   * @param {Object} session - MentraOS session
   */
  async handleForecastCommand(match, session) {
    // Example of using special symbols (arrows, degree) to show a weather forecast
    await this.displayController.sendCommand('clear');
    
    // Current temperature with degree symbol
    await this.displayController.sendCommand('textxy:5,5,Now: 72\\deg F');
    
    // Forecast using arrows for trend
    await this.displayController.sendCommand('textxy:5,15,Today: 68-75\\deg F \\up');
    await this.displayController.sendCommand('textxy:5,25,Tomorrow: 65-70\\deg F \\down');
    
    // Wind direction
    await this.displayController.sendCommand('textxy:5,35,Wind: NE 5mph \\right');
    
    // Update dashboard
    await this.dashboardController.updateDisplayContent(session, 'Weather Forecast');
    
    // Show in glasses
    await session.layouts.showTextWall('Weather forecast displayed with special symbols', { durationMs: 3000 });
    
    return { success: true };
  }
  
  /**
   * Handle show logo command
   * @param {Array} match - Regex match groups
   * @param {Object} session - MentraOS session
   */
  async handleShowLogo(match, session) {
    // Display logo on OLED
    await this.displayController.sendCommand('logo');
    
    // Update dashboard
    await this.dashboardController.updateDisplayContent(session, 'Logo');
    
    // Show in glasses
    await session.layouts.showTextWall('Logo displayed', { durationMs: 2000 });
    
    return { success: true };
  }
  
  /**
   * Handle directions command
   * @param {Array} match - Regex match groups
   * @param {Object} session - MentraOS session
   */
  async handleDirections(match, session) {
    // Show directions demo on OLED
    await this.displayController.sendCommand('directions');
    
    // Update dashboard
    await this.dashboardController.updateDisplayContent(session, 'Direction arrows');
    
    // Show in glasses
    await session.layouts.showTextWall('Direction arrows displayed', { durationMs: 3000 });
    
    return { success: true };
  }
  
  /**
   * Handle clear screen command
   * @param {Array} match - Regex match groups
   * @param {Object} session - MentraOS session
   */
  async handleClearScreen(match, session) {
    // Clear the OLED display
    await this.displayController.sendCommand('clear');
    
    // Update dashboard
    await this.dashboardController.updateDisplayContent(session, 'Screen cleared');
    
    // Show in glasses
    await session.layouts.showTextWall('Display cleared', { durationMs: 2000 });
    
    return { success: true };
  }
  
  /**
   * Handle help command
   * @param {Array} match - Regex match groups
   * @param {Object} session - MentraOS session
   */
  async handleHelp(match, session) {
    // Show help on the OLED
    await this.displayController.sendCommand('help');
    
    // Update dashboard
    await this.dashboardController.updateDisplayContent(session, 'Help displayed');
    
    // Create a list of all available commands for the glasses
    const commandList = this.commandPatterns
      .map(cmd => cmd.description)
      .join('\n');
    
    // Show help in glasses
    await session.layouts.showReferenceCard(
      'Available Voice Commands',
      commandList,
      { durationMs: 10000 }
    );
    
    return { success: true };
  }
  
  /**
   * Handle web search command
   * @param {Array} match - Regex match groups
   * @param {Object} session - MentraOS session
   */
  async handleWebSearch(match, session) {
    const query = match[1];
    
    if (!query || query.trim().length === 0) {
      await session.layouts.showTextWall('Please specify what to search for.', { durationMs: 3000 });
      return { success: false, error: 'Empty query' };
    }
    
    try {
      // Show searching status
      await this.displayController.sendCommand(`clear`);
      await this.displayController.sendCommand(`text:Searching for:`);
      await this.displayController.sendCommand(`textxy:0,15,${query.substring(0, 20)}...`);
      
      // Update dashboard
      await this.dashboardController.updateDisplayContent(session, `Searching: ${query}`);
      
      // Show searching message in glasses
      await session.layouts.showTextWall(`Searching for: ${query}`, { durationMs: 2000 });
      
      // Perform the search
      const searchResults = await this.webSearchService.searchWeb(query);
      
      // Update dashboard with search results
      await this.dashboardController.updateSearchResults(session, query, searchResults);
      
      if (searchResults.success) {
        // Format results for OLED display
        const displayText = this.webSearchService.formatResultsForDisplay(searchResults);
        
        // Display on OLED
        await this.displayController.sendCommand('clear');
        await this.displayController.sendCommand(`multiline:${displayText.replace(/\n/g, '\\n')}`);
        
        // Show results in glasses
        if (searchResults.results.answer) {
          await session.layouts.showTextWall(searchResults.results.answer, { durationMs: 8000 });
        } else {
          await session.layouts.showTextWall(`Search complete for: ${query}`, { durationMs: 3000 });
        }
        
        // Show sources in a reference card if available
        if (searchResults.results.sources && searchResults.results.sources.length > 0) {
          const sourcesText = searchResults.results.sources
            .map((source, index) => `${index + 1}. ${source.title}\n${source.url}`)
            .join('\n\n');
            
          await session.layouts.showReferenceCard('Search Sources', sourcesText, { durationMs: 10000 });
        }
        
        return {
          success: true,
          query,
          resultsCount: searchResults.results.sources?.length || 0
        };
      } else {
        // Handle search error
        await this.displayController.sendCommand('clear');
        await this.displayController.sendCommand(`multiline:Search Error:\\n${searchResults.error}`);
        
        await session.layouts.showTextWall(`Search error: ${searchResults.error}`, { durationMs: 3000 });
        
        return {
          success: false,
          error: searchResults.error
        };
      }
    } catch (error) {
      this.logger.error(`Error handling web search: ${error.message}`);
      
      await this.displayController.sendCommand('clear');
      await this.displayController.sendCommand(`multiline:Error:\\n${error.message}`);
      
      await session.layouts.showTextWall(`Error searching: ${error.message}`, { durationMs: 3000 });
      
      return {
        success: false,
        error: error.message
      };
    }
  }
}

export default VoiceCommandProcessor;
