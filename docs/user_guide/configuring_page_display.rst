
Configuring Page Display
=========================


This is one of the many new exciting features of Tripal v3.x. In this version of Tripal we have taken integration with Drupal Fields to a whole new level representing each piece of content (in Chado or otherwise) as a Drupal Field. What this means for site builders is unprecendented control over content display and arrangement through the administrative user interface --No more editing PHP template files to change the order, grouping or wording of content!

You can configure the display of a given Tripal Content Type by navigating to ``Structure → Tripal Content Types`` and then selecting the "Manage Display" link beside the content type you would like to configure.

.. image:: ./configuring_page_display.1.png



The Manage Display User Interface lists each Drupal Field in the order they will be displayed on the page. Fields are grouped into Tripal Panes by the Tripal DS module and the page is automatically divided into a right and left column. By default the left column contains the table of contents which lists the Tripal Panes available to the user in the order they are listed in this UI. The following screenshots are using the Analysis Content Type for demonstatration.

.. image:: configuring_page_display.2.png


Rearranging Fields
~~~~~~~~~~~~~~~~~~~

To rearrange the fields within a Tripal pane, simply drag them into the order you would like them. For example, the description is currently within the Summary table --it makes much more sense for it to be below the table but still within the summary. To do this, simply drag the description field to the bottom of the summary table and then move it in one level as shown in the following screenshot. Then click the **Save** button at the botton to save the changes.

.. image:: configuring_page_display.3.rearrange.png


Removing Fields and/or Field Lables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Now say we don't want the label "Description" in front of description content since it's pretty self explanatory. We can do that by changing the drop-down beside "Description" which currently says "Above" to "Hidden". This removes the label for the field assuming it's not within a table.

There may also be data you want to collect from your user but don't want to display on the page. This can be achomplished by disabling the field in the Manage Display UI. For example, we might not feel the need to tell users that this is an alaysis page and thus want to hide the Resource Type Field. This is done by changing the drop-down beside the Resource type field from "Right" to "Disabled".

.. warning::

  Don't forget to save the configuration often as you are changing it. You will not see changes to the page unless the **Save** button at the bottom of the Manage Display UI is clicked.

Changing Tripal Pane Names
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The name of a Tripal Pane is displayed both in the header of the Pane itself and in the Table of Contents. To change this name, click the gear button to the far right of the Tripal Pane you would like to change. This will bring up a blue pane of settings. Changing the Field Group Label will change the display name of the pane. For example, the following screenshot shows how you would change the "Cross References" Tripal Pane to be labeled "External Resources" instead if that it what you prefer. Then just click the Update button to see your changes take effect.

.. image:: ./configuring_page_display.4.png


Display/Hide Tripal Panes on Page Load
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can also easily control which Tripal Panes you would like displayed to the user on initial page load. By default the Summary Pane is the only one configured to show by default. However, if you would prefer for all panes or even a specific subset of panes to show by default, you can simply click the gear button to the far right of each Tripal Pane you want displayed by default and uncheck the "Hide panel on page load" checkbox. This gives you complete control over which panes you want your user to see first. If more then one pane is displayed by default then they will be shown in the order they are listed on the Manage Display UI.

Display/Hide Empty Fields
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

By default Tripal v3 hides all empty fields from the user. However like most behaviour in Tripal, this can be configured. If you would prefer to show all fields to the user regardless of whether there is content for that particular page, then navigate to ``Structure → Tripal Content Types`` and then click on the edit link beside the Tripal Content Type you would like to show empty fields for. Near the bottom of this form is a **Field Display** drop-down. Just change this drop-down to "show empty fields" and then click **Save Content Type**. As an example, we have changed this setting for the organism content type and, as you can see below, now you can see all fields (including empty fields like cross references and relationships) available to the organism content type.

.. image:: ./configuring_page_display.5.png
