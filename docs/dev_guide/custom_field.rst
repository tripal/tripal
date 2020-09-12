Creating a Custom Field
=======================

The most common way that new content will be added to an existing site is by creating new fields, or field displays.  In Tripal v2 customizations were added by editing PHP templates files.  These template files were  relatively easy to create and customize, but they provided less flexibility and did not integrate well with other Drupal features such as GUI-based page layout and Drupal Views.  Tripal v3 fields now provide this flexibility.  They also support data exchange and data collections!

By default Tripal v3 provides many fields for display of Chado data. However, you may find that these fields do not display data as you want, or you want to display data that the current fields do not already provide. This section of the Handbook describes how to create new fields that are integrated into the display, search and exchange abilities of both Drupal and Tripal.

If you are already familiar with Drupal fields you may be aware of the API functions and hooks that Drupal provides.  However, for the quantity of fields needed to support biological data, the Drupal API hooks quickly become overwhelming.  Additionally, documentation for fields in the Drupal API can sometimes be difficult to discover when first working with fields.   Therefore, Tripal provides several new PHP classes to simplify creation of fields and to consolidate all functionality into one easy to find set of files.  To develop new fields you should be somewhat familiar working with PHP's Object-Oriented Classes. The new classes provided by Tripal are these:


+----------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Class Name           | Description                                                                                                                                                                                                               |
+----------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| TripalField          | The TripalField class provides the basic information about a new field. It provides loaders for extracting data from the database and functions for querying data managed by the field.                                   |
+----------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| TripalFieldWidget    | The TripalFieldWidget class provides the necessary form elements when editing and Entity to allow the end-user to edit the value of the field (if desired). It provides the necessary validators and submitter functions. |
+----------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| TripalFieldFormatter | The TripalFieldFormatter class provides the visualization of the field when viewed on the page.                                                                                                                           |
+----------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| ChadoField           | The ChadoField class extends the TripalField class and provides the necessary settings to allow the field to map entities to data in Chado                                                                                |
+----------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| ChadoFieldWidget     | Extends the TripalFieldWidget class but currently provides no additional functionality. Use this class when working with Chado data to ensure future backwards compatibility.                                             |
+----------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| ChadoFieldFormatter  | Extends the TriplFieldFormatter class but currently provides no additional functionality. Use this class when working with Chado data to ensure future backwards compatibility.                                           |
+----------------------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+



The process for creating a custom field are as follows:

* Determine the controlled vocabulary term that best describes the data your field will create.
* Decide if you need the Chado field classes or the base Tripal field classes.  If you intend to work with data housed in Chado then you should use the Chado field classes.
* Decide if you want to build your class manually from the ground up or speed development by using the Staton Lab Fields Generator tool.
* Create new implementations of classes that extend those listed in the table above.  If you implement the functions properly your field is plug-and-play!  Tripal will find it and be able to use it.

The rest of this section will walk you through these steps.


.. toctree::
   :maxdepth: 2
   :caption: Table of Contents

   custom_field/select_vocab_terms
   custom_field/manual_field_creation
   custom_field/custom_widget
   custom_field/custom_formatter
   custom_field/ajax_custom_formatter
   custom_field/create_instance
   custom_field/tripal_field_generator
