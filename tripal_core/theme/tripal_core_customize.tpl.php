<p>Tripal was designed to allow for maiximum customization of content and the look-and-feel of the site.  By default,
Tripal provides a suite of Drupal style templates that provide a generic presentation of data from a Chado database. 
There are many options for customizing the look and feel of Tripal. Some customizations can be done without any
programming whatsoever.  However, for advanced customizations, Tripal provides a robust API to allow a programmer
to easily interact (select, update, delete, insert) data from a Chado database in whatever custom way desired.</p>

<li><p><b>Integration with Drupal Views</b>: <a href="http://drupal.org/project/views">Drupal Views</a> is
a powerful tool that allows the site administrator to create lists or basic searching forms of Chado content.
It provides a graphical interface within Drupal to allow the site admin to directly query the Chado database
and create custom lists without PHP programming or customization of Tripal source code.  Views can also
be created to filter content that has not yet been synced with Druapl in order to protect access to non
published data (only works if Chado was installed using Tripal).  If you have Views installed and enabled
You can see a list of available pre-existing
Views <a href="' . url('admin/build/views/') ?>">here</a>, as well as create your own. All Chado tables have 
been exposed to Drupal Views</p></li>

<h3>Customizations Without Programming</h3>
<p>There are several ways to customize the look-and-feel for the way Chado data is presented through Tripal.
Below is a description of several methods.  These methods may be used in conjunction with one another to
provide fine-grained control.</p>
<ul>

<li><p><b>Integration with Drupal Panels</b>:  <a href="http://drupal.org/project/views">Drupal Panels</a>
allows for customization of a page layout. Tripal comes with pre-set layouts for project pages.  However,
Panels become useful if you prefer a layout that is different from the pre-set layouts.  Chado content
is provided to Panels in the form of Drupal "blocks" which you can then place anywhere on a page using the
Panel\'s GUI.</p></li>

<li><p><b>Drupal\'s Content Construction Kit (CCK)</b>: the
<a href="http://drupal.org/project/cck">Content Construction Kit (CCK) </a> is a powerful way to add non-Chado content
to any page without need to edit template files or knowing PHP.  You must first download and install CCK.
With CCK, the site administartor can create a new fields to appear on any Tripal node page.  
A field added by CCK will appear in the form when editing to allow users to manually enter the appropriate
text.  These field values are not stored in Chado, but will appear on the page when saved. 
If you are using the default themeing provided by Tripal, then it is better to create the CCK element,
indicate that it is not to be shown (using the CCK interface), then manually add the new fields
where desired by editing the templates (see section below about customizing through programming).  
If using Panels, the CCK field can be added to the location desired using the Panels interface.</p></li>

<li><p><b>Adding Links to the "Resources" Sidebar</b>: You can add new items to the resources side-bar of any Tripal node using the 
<a href="http://drupal.org/project/cck">Content Construction Kit (CCK) </a>module and adding new field types to Tripal node types.  Instructions for adding these fields can be found
on the <?php l('Tripal v1.0 Tutorial', 'http://www.gmod.org/wiki/Tripal_Tutorial_(v1.0)#Adding_Additional_Resources')?>  </p>
</li>

</ul>

<h3>Customizations Through Programming</h3>
<ul>
<li><p><b>Adding Links to the "Resources" Sidebar</b>: If you use the pre-set default Tripal layout for theming, you
will see a "Resources" sidebar on each page.  The links that appear on the sidebar are automatically generated
using Javascript for all of the project "Blocks" that appear on the page. If you want to add additional links
(e.g. a link to a views table showing all features of the current project) and you want that link to appear in the
"Resources" sidebar, simply edit the Drupal Node Template (as described above) and add the link to the
section at the bottom of the template file where the resources section is found.</p></li>

<li><p><b>Editing Templates and Creating Custom Extension Modules</b>:There are several ways to customize the look-and-feel
  for the way Chado data is presented through Tripal.
  See the <a href="http://www.gmod.org/wiki/Tripal_Developer's_Handbook">Developers Handbook</a> for further infromation.   
</p></li>

<li><p><b>Sharing your customizations</b>: If you create a custom extension module that uses the Tripal API in
accordance with the instructions in the Developers Handbook you can share your modules with other Tripal users.
To share you module, you must first create a project account with Drupal and upload your extension module there. 
Next, subscribe to the <a href="https://lists.sourceforge.net/lists/listinfo/gmod-tripal-devel">Tripal Developer's Mailing List
</a> and send an email to the group indicating you have a new extension module and would like to share it.  A list of 
extension modules that properly follow the Tripal API will be listed on the Tripal website for others to easily find.</p></li>
</ul>
