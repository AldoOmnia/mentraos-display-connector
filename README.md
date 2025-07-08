# MentraOS OLED Display Connector

[![CI/CD Pipeline](https://github.com/yourusername/mentraos-display-connector/actions/workflows/ci.yml/badge.svg)](https://github.com/yourusername/mentraos-display-connector/actions/workflows/ci.yml)

This project integrates a CFAL12856A0-0151-B OLED display (controlled by an Arduino) with MentraOS for smart glasses. It enables displaying information from your MentraOS smart glasses app on an external OLED display via serial communication.

## Project Overview

The MentraOS OLED Display Connector acts as a bridge between:
- MentraOS smart glasses applications
- An Arduino-connected CFAL12856A0-0151-B OLED display 

The application allows text, symbols, and simple graphics to be displayed on the OLED based on voice commands or interactions from MentraOS smart glasses.

![MentraOS Display Example](https://via.placeholder.com/640x320/text=MentraOS+Display+Example)

## Features

- Display text from MentraOS on the OLED display
- Support for special symbols (arrows, degree symbol, etc.)
- Support for positioning text with x,y coordinates
- Support for multiline text
- Support for scrolling text
- Display predefined graphics (logos, icons, aiming reticle)
- Advanced weather display with temperature and forecast symbols
- **Web search functionality powered by Perplexity AI**
- Omnia-like spatial browser capabilities
- Voice command support via MentraOS transcription
- Button press support from MentraOS smart glasses
- Robust error handling and reconnection logic
- MentraOS dashboard integration with real-time status

## Prerequisites

- Node.js (v18 or later)
- Bun runtime (for faster execution)
- Arduino with CFAL12856A0-0151-B OLED display running the compatible firmware
- MentraOS compatible smart glasses
- MentraOS developer account
- ngrok (for local testing)

## Setup

### Arduino Setup

1. Flash the Arduino sketch to your Arduino board:
   - Open `/arduino/CFAL12856A00151B.ino` in Arduino IDE
   - Connect your Arduino to your computer
   - Select the correct board and port
   - Upload the sketch to your Arduino

2. Connect the CFAL12856A0-0151-B OLED display to the Arduino following the wiring diagram:
   ```
   Arduino       OLED Display
   -------       ------------
   GND    -----> GND
   3.3V   -----> VCC
   D13    -----> SCK
   D11    -----> MOSI
   D10    -----> CS
   D9     -----> D/C
   D8     -----> RESET
   ```

### Cloud App Setup

1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/mentraos-display-connector.git
   cd mentraos-display-connector
   ```

2. Install dependencies:
   ```bash
   # Using npm
   npm install
   
   # OR using Bun (recommended)
   bun install
   ```

3. Configure environment variables:
   ```bash
   cp .env.example .env
   ```
   Edit the `.env` file with your specific configuration:
   - Set `SERIAL_PORT` to the Arduino's serial port (e.g., `/dev/tty.usbserial-1410`, `/dev/ttyACM0`, or `COM3`)
   - Set `SERIAL_BAUD_RATE` to match the Arduino sketch (default: 9600)
   - Set `PACKAGE_NAME` to your MentraOS application package name (e.g., `com.yourdomain.displayapp`)
   - Set `MENTRAOS_API_KEY` to your MentraOS API key from the developer console
   - Set `PERPLEXITY_API_KEY` to your Perplexity API key for web search functionality

4. Register your app in the MentraOS developer console:
   - Go to [console.mentra.glass](https://console.mentra.glass/)
   - Register a new app with your package name
   - Generate an API key and add it to your `.env` file
   - Note: You'll set the webhook URL after starting ngrok in the next steps

## Running the Application

### Local Development

1. Start the application:
   ```bash
   # Using npm
   npm run dev
   
   # OR using Bun
   bun run dev
   ```

2. In a separate terminal, start ngrok to expose your local server:
   ```bash
   npm run start:ngrok
   # OR
   ngrok http 3000
   ```

3. Update your app's public URL in the MentraOS developer console to your ngrok URL

4. Start the app on your MentraOS smart glasses

### Production Deployment

1. Deploy to your preferred cloud provider (Heroku, AWS, etc.)

2. Set the environment variables in your cloud provider's dashboard

3. Update your app's public URL in the MentraOS developer console

## Available Voice Commands

The app supports the following voice commands:

### Display Commands
- "Show text [message]" - Display text on the OLED
- "Show temperature" - Display temperature with degree symbol
- "Show time" - Display current time
- "Show weather" - Display weather demo with symbols
- "Show forecast" - Display detailed weather forecast with symbols
- "Show logo" - Display logo
- "Show directions" - Display direction arrows
- "Clear screen" - Clear the display
- "Help" or "Show commands" - Display list of commands

### Web Search Commands (Omnia Spatial Browser-like)
- "Search for [topic]" - Search the web for information
- "Look up [topic]" - Search the web for information
- "Find [topic]" - Search the web for information
- "What is [topic]" - Get information about a topic
- "Who is [person]" - Get information about a person
- "Tell me about [topic]" - Get detailed information about a topic

## Button Controls

The following button actions are supported:

- SELECT - Clear screen and show a message
- BACK - Show index screen
- UP - Show directions demo
- DOWN - Show weather demo

## Arduino Serial Commands

The Arduino accepts the following serial commands:

- `text:message` - Display text
- `textxy:x,y,message` - Display text at position
- `multiline:line1\nline2` - Display multiline text
- `scroll:message` - Scroll text
- `directions` - Show direction arrows
- `weather` - Show weather with degree symbol
- `aiming` - Show aiming reticle bitmap
- `eagle` - Show eagle bitmap
- `clear` - Clear display
- `show_logo` - Show logo
- `show_index` - Show index
- `demo` - Run demo sequence
- `help` - Show help

## Repository Structure

```
mentraos-display-connector/
├── .github/              # GitHub Actions workflows
├── arduino/              # Arduino sketches and related files
│   └── CFAL12856A00151B.ino  # Main Arduino sketch
│   └── font5x8.h         # Font bitmap definitions
├── src/
│   ├── dashboard/        # MentraOS dashboard integration
│   ├── display/          # Serial display controller
│   ├── utils/            # Utilities (logger, etc.)
│   ├── voice/            # Voice command processing
│   └── index.js          # Main application entry point
├── .env.example          # Example environment variables
├── .gitignore            # Git ignore file
├── package.json          # Node.js dependencies
└── README.md             # This documentation
```

## Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Commit your changes: `git commit -m 'Add feature'`
4. Push to the branch: `git push origin feature-name`
5. Submit a pull request

## License

MIT License - See LICENSE file for details

## Acknowledgments

- MentraOS for their smart glasses platform
- Crystalfontz for the OLED display
