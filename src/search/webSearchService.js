import axios from 'axios';
import { convert } from 'html-to-text';
import Logger from '../utils/logger.js';

/**
 * Web search functionality powered by Perplexity AI
 */
class WebSearchService {
  constructor() {
    this.logger = new Logger('WebSearchService');
    this.API_KEY = process.env.PERPLEXITY_API_KEY;
    this.API_URL = 'https://api.perplexity.ai';
    
    if (!this.API_KEY) {
      this.logger.warn('Perplexity API key not found in environment variables. Web search functionality will be limited.');
    }
  }
  
  /**
   * Search the web using Perplexity AI API
   * @param {string} query - Search query
   * @returns {Promise<Object>} - Search results
   */
  async searchWeb(query) {
    try {
      this.logger.info(`Performing web search: "${query}"`);
      
      if (!this.API_KEY) {
        return this.mockSearchResults(query);
      }
      
      const response = await axios.post(`${this.API_URL}/search`, {
        query,
        max_results: 3,
        include_answer: true,
        include_images: false
      }, {
        headers: {
          'Authorization': `Bearer ${this.API_KEY}`,
          'Content-Type': 'application/json'
        }
      });
      
      // Extract and format the results
      const results = {
        answer: response.data.answer || '',
        sources: response.data.results.map(result => ({
          title: result.title,
          url: result.url,
          snippet: result.snippet
        }))
      };
      
      this.logger.info(`Found ${results.sources.length} results for query: "${query}"`);
      
      return {
        success: true,
        results
      };
    } catch (error) {
      this.logger.error(`Error in web search: ${error.message}`);
      return {
        success: false,
        error: error.message
      };
    }
  }
  
  /**
   * Format search results for OLED display
   * @param {Object} searchResults - Results from searchWeb
   * @returns {string} - Formatted text for display
   */
  formatResultsForDisplay(searchResults) {
    if (!searchResults.success) {
      return `Search error: ${searchResults.error}`;
    }
    
    let displayText = '';
    
    // Add summarized answer if available
    if (searchResults.results.answer) {
      const summary = searchResults.results.answer.substring(0, 150);
      displayText += `${summary}...\n\n`;
    }
    
    // Add sources
    if (searchResults.results.sources && searchResults.results.sources.length > 0) {
      displayText += 'Sources:\n';
      
      searchResults.results.sources.forEach((source, index) => {
        const snippetText = convert(source.snippet || '', {
          wordwrap: 20,
          limits: {
            maxInputLength: 100
          }
        });
        
        displayText += `${index + 1}. ${source.title.substring(0, 20)}\n`;
      });
    }
    
    return displayText;
  }
  
  /**
   * Provide mock results when API key is not available
   * @param {string} query - Search query
   * @returns {Object} - Mock search results
   */
  mockSearchResults(query) {
    this.logger.warn('Using mock search results (no API key)');
    
    return {
      success: true,
      results: {
        answer: `This is a simulated answer about "${query}". To enable real search, please add your Perplexity API key to the .env file.`,
        sources: [
          {
            title: 'Sample Source 1',
            url: 'https://example.com/1',
            snippet: 'This is a sample search result. Add a Perplexity API key for real results.'
          },
          {
            title: 'Sample Source 2',
            url: 'https://example.com/2',
            snippet: 'Another sample result for demonstration purposes.'
          }
        ]
      }
    };
  }
}

export default WebSearchService;
