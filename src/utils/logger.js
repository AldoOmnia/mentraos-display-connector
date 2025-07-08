import pino from 'pino';

/**
 * Logger utility for consistent logging across the application
 */
class Logger {
  constructor(moduleName) {
    this.logger = pino({
      transport: {
        target: 'pino-pretty',
        options: {
          colorize: true,
          translateTime: 'SYS:standard',
          ignore: 'pid,hostname',
        },
      },
      name: moduleName,
      level: process.env.NODE_ENV === 'development' ? 'debug' : 'info',
    });
  }

  debug(message, context = {}) {
    this.logger.debug(context, message);
  }

  info(message, context = {}) {
    this.logger.info(context, message);
  }

  warn(message, context = {}) {
    this.logger.warn(context, message);
  }

  error(message, context = {}) {
    if (context instanceof Error) {
      this.logger.error({ err: context }, message);
    } else if (typeof context === 'string') {
      this.logger.error(message + ': ' + context);
    } else {
      this.logger.error(context, message);
    }
  }
}

export default Logger;
