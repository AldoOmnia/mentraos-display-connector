import Logger from '../utils/logger.js';

/**
 * Handles voice command processing for the MentraOS app
 */
class VoiceCommandProcessor {
  constructor(displayController, dashboardController) {
    this.displayController = displayController;
    this.dashboardController = dashboardController;
    this.logger = new Logger('VoiceCommandProcessor');
    
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
   * @param {Object} session - MentraOS session object
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
}

export default VoiceCommandProcessor;
