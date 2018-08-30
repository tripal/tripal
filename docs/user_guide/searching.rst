Searching
===============

Drupal and Tripal offer a variety of methods for searching biological content on your site.  Each has it's own advantages and meets different needs.  This section provides a description of several different ways to add searching.   The two primary categories of search tools are content-specific and site-wide searching.  The site-wide search tools typically provide a single text box that allow a user to provide a set of key words.  The results of the search will span multiple content types.  Often, site-wide searches allow users to quickly find content regardless of the content type.  But, they sometimes lack fine-grained control for filtering.  The content-specific search tools provide more fine-grained filtering for a single content type.  Therefore, it is often necessary to provide multiple types of search tools for different content types.

There are several options for the addition of both site-wide and content-specific search tools which include:

**For site-wide searching you can:**

- Use the Default Drupal Search
- Use the Search API Module
- Use an independent search tool. Two popular tools that integrate with Drupal include:
  - ElasticSearch
  - Apache Solr

**For content-specific searching you can:**

- Use the search tools that Tripal provides
- Develop your own search tools using Drupal Views
- Write your own custom search tools using PHP and Tripal's API functions.

You may not want to consider using multiple search tools, such as a site-wide tool and content-specific tools.  The following sections provide a description for use and setup of some of these different options.


.. toctree::
   :maxdepth: 1
   :caption: Search Guide

   ./searching/default_pages
   ./searching/search_api
   ./searching/elasticsearch_module
