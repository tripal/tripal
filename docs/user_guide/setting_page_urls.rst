Setting Page Titles and URLs
=============================

Tripal allows for Page Titles and URLs to be set within the Tripal Content type editing interface. This provides the ability to construct consistent url patterns and titles across your site.

Setting Page Titles
=====================

Page titles can be set within the edit mechanism of the Tripal Content type. This can be found on the ``Structure → Tripal Content Types  → <specific content type>``. Scroll to the bottom of the page to the "Page Title options" tab.

.. image:: ./setting_page_urls.1.png


Page Title Settings
~~~~~~~~~~~~~~~~~~~~

Then the page title pattern can be generated using combination of token. The tokens can be found under the 'Available Tokens' link. Keep in mind that it might be confusing to users if more than one page has the same title.

.. note::

	We recommend you choose a combination of tokens that will uniquely identify your content.

If you already have content within your site and need to update all page titles you can choose to 'Bulk update all titles'. This will update all existing titles for the content type in question. If your title is used to build your alias you will also need to 'Bulk update all aliases'.

Setting URLs
=============

URLs, also known as aliases, can you found just below the Page Title options tab.The url pattern can be generated using combination of token. The tokens can be found under the 'Available Tokens' link. If you already have content within your site and need to update all urls you can choose to 'Bulk update all aliases'. This will update all existing urls for the content type in question. It will also create redirects from the old url to the new url to ensure 404s and broken links are not created.


.. image:: ./setting_page_urls.2.png
