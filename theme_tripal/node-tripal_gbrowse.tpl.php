<?php
// Developed by: Chad N.A Krilow at The University of Saskatchewan
//
// Purpose: This template provides the layout of the Tripal GBrowse Instances node (page)
//   using the same templates used for the various Tripal GBrowse blocks. An I-Frame is 
//	 included to show the representation of the GBrowse instance(including tracks). Along with 
//	 the I-Frame, a link to open a external I-Frame in a new window is supplied
//
// To Customize the Stock Node Page:
//   - This Template: customize basic layout and which elements are included
//   - Using Panels: Override the node page using Panels3 and place the blocks
//       of content as you please. This method requires no programming. See
//       the Tripal User Guide for more details
//
// Variables Available:
//   - $node: a standard object which contains all the fields associated with
//       nodes and it also includes Tripal GBrowse specific fields.
//
//   NOTE: For a full listing of fields available in the node object the
//       print_r $node line below or install the Drupal Devel module which 
//       provides an extra tab at the top of the node page labelled Devel
?>


<?php
 //uncomment this line to see a full listing of the fields avail. to $node
 //print '<pre>'.print_r($node,TRUE).'</pre>';
 drupal_add_css('./tripal-node-templates.css');
?>

<?php if ($teaser) { 
  
  include('tripal_gbrowse/tripal_gbrowse_teaser.tpl.php');
} else { ?>
	  
<script type="text/javascript">
 if (Drupal.jsEnabled) {
   $(document).ready(function() {
      // hide all tripal info boxes at the start
      $(".tripal-info-box").hide();
 
      // iterate through all of the info boxes and add their titles
      // to the table of contents
      $(".tripal-info-box-title").each(function(){
        var parent = $(this).parent();
        var id = $(parent).attr('id');
        var title = $(this).text();
        $('#tripal_gbrowse_toc_list').append('<li><a href="#'+id+'" class="tripal_gbrowse_toc_item">'+title+'</a></li>');
      });

      // when a title in the table of contents is clicked, then
      // show corresponding item in details box
      $(".tripal_gbrowse_toc_item").click(function(){
         $(".tripal-info-box").hide();
         href = $(this).attr('href');
         $(href).fadeIn('slow');
         // make sure table of contents and the details
         // box stay the same height
         $("#tripal_gbrowse_toc").height($(href).parent().height());
         return false;
      }); 

      // base details show up when the page is first shown 
      // unless the user specified a specific block
      var block = window.location.href.match(/\?block=.*/);
      if(block != null){
         block_title = block.toString().replace(/\?block=/g,'');
         $("#tripal_gbrowse-"+block_title+"-box").show();
      } else {
         $("#tripal_gbrowse-base-box").show();
      }

      $("#tripal_gbrowse_toc").height($("#tripal_gbrowse-base-box").parent().height());
   });
}
</script>

<style type="text/css">
  /* these styles are specific for this template and is not included 
     in the main CSS files for the theme as it is anticipated that the
     elements on this page may not be used for other customizations */
  #tripal_gbrowse_toc {
     float: left;
     width: 20%;
     background-color: #EEEEEE;
     -moz-border-radius: 15px;
     border-radius: 15px;
     -moz-box-shadow: 3px 3px 4px #888888;
	  -webkit-box-shadow: 3px 3px 4px #888888;
	  box-shadow: 3px 3px 4px #888888;
     padding: 20px;
     min-height: 200px;
     border-style:solid;
     border-width:1px;
  }
  #tripal_gbrowse_toc ul {
    margin-left: 0px;
    margin-top: 5px;
    padding-left: 15px;
  }
  #tripal_gbrowse_toc_title {
     font-size: 1.5em;
  }
  #tripal_gbrowse_toc_desc {
    font-style: italic;
  }
  #tripal_gbrowse_details {
     float: left;
     width: 70%;
     background-color: #FFFFFF;
     -moz-border-radius: 15px;
     border-radius: 15px;
     -moz-box-shadow: 3px 3px 4px #888888;
	  -webkit-box-shadow: 3px 3px 4px #888888;
	  box-shadow: 3px 3px 4px #888888;
     padding: 20px;
     min-height: 200px;
     margin-right: 10px;
     border-style:solid;
     border-width:1px;
  }
  #tripal_gbrowse-base-box img {
    float: left;
    margin-bottom: 10px;
  }
  #tripal_gbrowse-table-base {
    float: left;
    width: 400px;
    margin-left: 10px;
    margin-bottom: 10px;
  }
  #tripal_gbrowse_addtional_content {
		clear:both;
  	width: 100%;
  }
</style>

<div id ="tripal_gbrowse_content">
<div id="tripal_gbrowse_details" class="tripal_details"><h2>

   <!-- Basic Details Theme -->
   <?php include('tripal_gbrowse/tripal_gbrowse_details.tpl.php'); ?>

   <!-- GBrowse Details -->
   <?php 
   global $account;
   if(user_access('access database-related details',$account)){
     include('tripal_gbrowse/tripal_gbrowse_database_details.tpl.php'); 
	 }?>
   <?php print $content ?>
   
    <!-- GBrowse Loaded Sources -->
   <?php 
     include('tripal_gbrowse/tripal_gbrowse_loaded_sources.tpl.php');
     ?>
   <?php print $content ?>
 
</div>

<!-- Table of contents -->
<div id="tripal_gbrowse_toc" class="tripal_toc">
   <div id="tripal_gbrowse_toc_title" class="tripal_toc_title">Resources</div>
   <span id="tripal_gbrowse_toc_desc" class="tripal_toc_desc"></span>
   <ul id="tripal_gbrowse_toc_list" class="tripal_toc_list">

   </ul>
</div>
</div>

<!--- I-Frame Code Section --->
<div id="tripal_gbrowse_addtional_content">

	<!--- Include for file that makes the I-Frame and external window possible --->
 <?php include('tripal_gbrowse/tripal_gbrowse_gbrowse_instance.tpl.php'); ?>

</div>
<?php } ?>
