import Logger from '../utils/logger.js';

/**
 * Controller for managing MentraOS dashboard content
 */
class DashboardController {
  constructor() {
    this.logger = new Logger('DashboardController');
  }

  /**
   * Initialize dashboard content for a session
   * @param {Object} session - MentraOS session object
   */
  async initializeDashboard(session) {
    try {
      this.logger.info('Initializing dashboard content');
      
      // Get current timestamp for uptime tracking
      const startTime = new Date().toISOString();
      
      // Set initial dashboard content
      await session.dashboard.setContent({
        main: {
          title: 'OLED Status',
          value: 'Connected',
          color: '#00ff00' // Green for connected
        },
        expanded: {
          sections: [
            {
              title: 'OLED Display',
              items: [
                { label: 'Status', value: 'Connected' },
                { label: 'Display Size', value: '128x56' },
                { label: 'Interface', value: 'Serial' },
                { label: 'Model', value: 'CFAL12856A0-0151-B' },
                { label: 'Controller', value: 'SSD1309' },
                { label: 'Connected Since', value: startTime }
              ]
            },
            {
              title: 'Available Commands',
              items: [
                { label: 'Text', value: 'Display text messages' },
                { label: 'Weather', value: 'Show weather data with symbols' },
                { label: 'Directions', value: 'Display directional arrows' },
                { label: 'Time', value: 'Show current time' }
              ]
            }
          ]
        }
      });
      
      this.logger.info('Dashboard content set successfully');
    } catch (error) {
      this.logger.error('Failed to set dashboard content', error);
    }
  }
  
  /**
   * Update the dashboard with display status
   * @param {Object} session - MentraOS session object
   * @param {string} status - Status text to display
   * @param {string} color - Status color (hex code)
   */
  async updateStatus(session, status, color = '#00ff00') {
    try {
      await session.dashboard.updateContent({
        main: {
          title: 'OLED Status',
          value: status,
          color: color
        }
      });
      
      // Also update the expanded view
      const expandedContent = await session.dashboard.getContent();
      if (expandedContent && expandedContent.expanded && expandedContent.expanded.sections) {
        // Update the status in the expanded view
        expandedContent.expanded.sections[0].items[0].value = status;
        
        await session.dashboard.updateContent({
          expanded: expandedContent.expanded
        });
      }
      
      this.logger.info(`Dashboard status updated: ${status}`);
    } catch (error) {
      this.logger.error('Failed to update dashboard status', error);
    }
  }
  
  /**
   * Update dashboard with the last displayed content
   * @param {Object} session - MentraOS session object
   * @param {string} content - Content displayed on OLED
   */
  async updateDisplayContent(session, content) {
    try {
      // Get current expanded content
      const currentContent = await session.dashboard.getContent();
      
      // If we don't have expanded content yet, create it
      if (!currentContent || !currentContent.expanded || !currentContent.expanded.sections) {
        return;
      }
      
      // Add or update the "Last Displayed" item
      let lastDisplayedFound = false;
      
      for (const item of currentContent.expanded.sections[0].items) {
        if (item.label === 'Last Displayed') {
          item.value = content.length > 20 ? content.substring(0, 17) + '...' : content;
          lastDisplayedFound = true;
          break;
        }
      }
      
      // If not found, add it
      if (!lastDisplayedFound) {
        currentContent.expanded.sections[0].items.push({
          label: 'Last Displayed',
          value: content.length > 20 ? content.substring(0, 17) + '...' : content
        });
      }
      
      // Update the expanded view
      await session.dashboard.updateContent({
        expanded: currentContent.expanded
      });
      
      this.logger.info('Dashboard display content updated');
    } catch (error) {
      this.logger.error('Failed to update dashboard display content', error);
    }
  }
  
  /**
   * Clear the dashboard content when session ends
   * @param {Object} session - MentraOS session object
   */
  async clearDashboard(session) {
    try {
      this.logger.info('Clearing dashboard content');
      
      await session.dashboard.clearContent();
    } catch (error) {
      this.logger.error('Failed to clear dashboard content', error);
    }
  }
}

export default DashboardController;
