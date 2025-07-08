import { SerialPort } from 'serialport';
import Logger from '../utils/logger.js';

/**
 * Controller for interacting with the CFAL12856A0-0151-B OLED display
 * connected to an Arduino via serial port
 */
class SerialDisplayController {
  constructor(options = {}) {
    this.port = options.port;
    this.baudRate = options.baudRate || 9600;
    this.width = options.width || 128;
    this.height = options.height || 56;
    this.autoReconnect = options.autoReconnect !== false;
    this.reconnectInterval = options.reconnectInterval || 5000;
    this.logger = new Logger('SerialDisplayController');
    this.serialPort = null;
    this.isConnected = false;
    this.commandQueue = [];
    this.isProcessingQueue = false;
  }
  
  /**
   * Initialize the serial connection to the Arduino
   */
  async initialize() {
    try {
      this.logger.info(`Initializing serial connection to ${this.port} at ${this.baudRate} baud`);
      
      this.serialPort = new SerialPort({
        path: this.port,
        baudRate: this.baudRate,
        autoOpen: false
      });
      
      // Set up event listeners
      this.serialPort.on('open', () => {
        this.isConnected = true;
        this.logger.info('Serial port opened successfully');
      });
      
      this.serialPort.on('close', () => {
        this.isConnected = false;
        this.logger.warn('Serial port closed');
        
        if (this.autoReconnect) {
          this.logger.info(`Will attempt to reconnect in ${this.reconnectInterval}ms`);
          setTimeout(() => this.reconnect(), this.reconnectInterval);
        }
      });
      
      this.serialPort.on('error', (err) => {
        this.logger.error(`Serial port error: ${err.message}`);
        this.isConnected = false;
      });
      
      this.serialPort.on('data', (data) => {
        const response = data.toString().trim();
        if (response) {
          this.logger.debug(`Received from Arduino: ${response}`);
        }
      });
      
      // Open the serial connection
      return new Promise((resolve, reject) => {
        this.serialPort.open((err) => {
          if (err) {
            this.logger.error(`Failed to open serial port: ${err.message}`);
            reject(err);
          } else {
            this.logger.info('Serial port connection established');
            
            // Wait a bit for Arduino to reset after connection
            setTimeout(async () => {
              // Send initial commands
              await this.sendCommand('reset');
              await this.sendCommand('clear');
              this.logger.info('Display initialized');
              resolve(true);
            }, 2000);
          }
        });
      });
    } catch (error) {
      this.logger.error(`Initialization error: ${error.message}`);
      throw error;
    }
  }
  
  /**
   * Attempt to reconnect to the serial port
   */
  async reconnect() {
    if (this.isConnected) return;
    
    try {
      this.logger.info('Attempting to reconnect...');
      await this.initialize();
    } catch (error) {
      this.logger.error(`Reconnection failed: ${error.message}`);
      
      if (this.autoReconnect) {
        this.logger.info(`Will attempt again in ${this.reconnectInterval}ms`);
        setTimeout(() => this.reconnect(), this.reconnectInterval);
      }
    }
  }
  
  /**
   * Send a raw command to the Arduino
   * @param {string} command - The command string to send
   * @returns {Promise<boolean>} - Success status
   */
  async sendCommand(command) {
    return new Promise((resolve, reject) => {
      if (!this.isConnected || !this.serialPort) {
        this.logger.warn(`Not connected, queuing command: ${command}`);
        this.commandQueue.push({ command, resolve, reject });
        this.processQueue();
        return;
      }
      
      this.logger.debug(`Sending command: ${command}`);
      
      // Add retry logic for reliability
      let retryCount = 0;
      const maxRetries = 3;
      
      const attemptSend = () => {
        this.serialPort.write(`${command}\n`, (err) => {
          if (err) {
            this.logger.error(`Error sending command: ${err.message}`);
            
            // Retry logic
            if (retryCount < maxRetries) {
              retryCount++;
              this.logger.warn(`Retrying command (${retryCount}/${maxRetries}): ${command}`);
              setTimeout(attemptSend, 500 * retryCount);
            } else {
              reject(new Error(`Failed to send command after ${maxRetries} attempts: ${err.message}`));
            }
          } else {
            // Wait a moment for the command to be processed
            setTimeout(() => resolve(true), 100);
          }
        });
      };
      
      // Start the first attempt
      attemptSend();
    });
  }
  
  /**
   * Process queued commands when connection is restored
   */
  async processQueue() {
    if (this.isProcessingQueue || !this.isConnected || this.commandQueue.length === 0) return;
    
    this.isProcessingQueue = true;
    this.logger.info(`Processing ${this.commandQueue.length} queued commands`);
    
    while (this.commandQueue.length > 0 && this.isConnected) {
      const { command, resolve, reject } = this.commandQueue.shift();
      try {
        await this.sendCommand(command);
        resolve(true);
      } catch (error) {
        reject(error);
      }
    }
    
    this.isProcessingQueue = false;
  }
  
  /**
   * Display text on the OLED at the specified position
   * @param {string} text - The text to display
   * @param {number} x - X position (0-127)
   * @param {number} y - Y position (0-55)
   */
  async displayText(text, x = 0, y = 0) {
    const command = `textxy:${x},${y},${text}`;
    return this.sendCommand(command);
  }
  
  /**
   * Display multi-line text on the OLED
   * @param {string} text - The text to display (can include \n)
   */
  async displayMultilineText(text) {
    const command = `multiline:${text}`;
    return this.sendCommand(command);
  }
  
  /**
   * Clear the display
   */
  async clearDisplay() {
    return this.sendCommand('clear');
  }
  
  /**
   * Display scrolling text
   * @param {string} text - The text to scroll
   */
  async scrollText(text) {
    const command = `scroll:${text}`;
    return this.sendCommand(command);
  }
  
  /**
   * Close the serial connection
   */
  async close() {
    return new Promise((resolve) => {
      if (!this.serialPort || !this.isConnected) {
        resolve();
        return;
      }
      
      this.logger.info('Closing serial connection...');
      this.serialPort.close(() => {
        this.isConnected = false;
        this.logger.info('Serial connection closed');
        resolve();
      });
    });
  }
}

export default SerialDisplayController;
