<?php
// $Id: node.tpl.php,v 1.3 2010/04/12 10:04:07 antsin Exp $

/*
+----------------------------------------------------------------+
|   BlogBuzz for Dupal 6.x - Version 1.0                         |
|   Copyright (C) 2009 Antsin.com All Rights Reserved.           |
|   @license - GNU GENERAL PUBLIC LICENSE                        |
|----------------------------------------------------------------|
|   Theme Name: BlogBuzz                                         |
|   Description: BlogBuzz by Antsin                              |
|   Author: Antsin.com                                           |
|   Website: http://www.antsin.com/                              |
|----------------------------------------------------------------+
*/  
?>

<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?>"><div class="node-inner">
  <div class="content">
    <?php print $picture; ?>
    <?php if ($submitted): ?>
      <div class="submitted">
        <?php print $date ?>
      </div>
    <?php endif; ?>
    <h1 class="title">
      <a href="<?php print $node_url; ?>" title="<?php print $title ?>"><?php print $title; ?></a>
    </h1>

    <?php if ($unpublished): ?>
      <div class="unpublished"><?php print t('Unpublished'); ?></div>
    <?php endif; ?>

    <div class="detail clear-block">

<!-- Tripal Stock Starts --!>

<?php if ($node->is_obsolete == 't') { print "<h4><i>This Stock is obsolete.</i></h4>"; } ?>
<table>
  <?php if(!empty($node->stock_name)) { ?>
    <tr><th>Name</th><td><?php print $node->stock_name; ?></td></tr>
  <?php } ?>
  <tr><th>Uniquename</th><td><?php print $node->uniquename; ?></td></tr>
  <tr><th>Organism</th><td><?php print l($node->organism->common_name, "node/".$node->organism->nid); print " (<i>". $node->organism->genus ." ". $node->organism->species ."</i>)"; ?></td></tr>
  <tr><th>Type</th><td><?php print $node->stock_type; ?></td></tr>
  <?php if (!empty($node->main_db_reference->db_name)) { ?>
    <tr><th>Database (<?php print $node->main_db_reference->db_name; ?>)</th>
      <?php if( !empty($node->main_db_reference->urlprefix) ) { ?>
        <td><?php print l($node->main_db_reference->accession, $node->main_db_reference->urlprefix.$node->main_db_reference->accession); ?></td></tr>
      <?php } else { ?>
	<td><?php print $node->main_db_reference->accession; ?></td></tr>
      <?php } 
  } ?>
  <?php if(!empty($node->description)) { ?>
    <tr><th>Description</th><td><?php print $node->description; ?></td></tr>
  <?php } ?>
  <?php if(!empty($node->crossingblock->season) && ($node->crossingblock->season != '---')) { ?>
    <tr><th>Crossing Block</th><td><?php print $node->crossingblock->year.' '.$node->crossingblock->season; ?></td></tr>
  <?php } ?>
</table>

<!-- Start of Expandable Boxes -->
   <?php if (!$teaser) { ?>
     <!-- Control link for the expandableBoxes -->
       <br><a id="tripal_expandableBox_toggle_button" onClick="toggleExpandableBoxes()">[-] Collapse All</a><br><br>
     <!-- End of Control link for the expandableBoxes -->

     <!-- Display of Chado Stock Properties from table stockprop in Chado --!>
     <?php $properties = $node->properties; 
     if ( (sizeof($node->synonyms) + sizeof($properties)) > 0 ) { ?>
     <div id="feature-references" class="tripal_feature-info-box">
     <div class="tripal_expandableBox"><h3>Properties</h3></div>
     <div class="tripal_expandableBoxContent">
     <table>
       <?php if(!empty($node->synonyms)) { ?>
         <tr><th>Synonyms</th><td>
           <?php print $node->synonyms[0]->value;
           array_shift($node->synonyms);
           if (sizeof($node->synonyms) >= 1) {
             foreach($node->synonyms as $synonym) {print ", ".$synonym->value;}
           } ?>
	    </td></tr>
       <?php } ?>
      </table>
      <table>
       <?php if (sizeof($properties) > 0) { ?>
         <tr><th>Type of Property</th><th>Value</th></tr>
         <?php foreach ($properties as $result) { ?>
           <tr><td><?php print $result->type; ?></td>
	      <td><?php  if( $result->value == 't') { print 'TRUE';
	           } elseif ($result->value == 'f') { print "FALSE";
		        } else { print $result->value; } ?>
			   </td></tr>
         <?php } 
	   } ?>
     </table>
     </div></div>
     <br>
     <?php } ?>
     <br>

    <!-- Display of External Database entries for the current stock --!>
    <?php $references = $node->db_references; 
    if ( count($references) > 0 ) { ?>
      <div id="feature-references" class="tripal_feature-info-box">
      <div class="tripal_expandableBox"><h3>External References</h3></div>
      <div class="tripal_expandableBoxContent">
      <table>
        <tr><th>Database</th><th>Accession</th></tr>
        <?php foreach($references as $result) { ?>
	  <tr><td><?php print $result->db_name; ?></td><td>
	      <?php if ( !empty($result->db_urlprefix) ) {
	            print l($result->accession, $result->db_urlprefix.$result->accession); 
		        } else {
			      print $result->accession;
			          }?>
				    </td></tr>
				    <?php } ?>
      </table></div></div>
      <br>
    <?php } ?>
    <br>

    <!-- Display of Relationships between this stock and other stocks --!>
    <?php $o_relationships = $node->object_relationships; ?>
    <?php $s_relationships = $node->subject_relationships; ?>
    <?php if ( (count($o_relationships) + count($s_relationships)) > 0 ) { ?>
      <div id="feature-references" class="tripal_feature-info-box">
      <div class="tripal_expandableBox"><h3>Relationships</h3></div>
      <div class="tripal_expandableBoxContent">
      <table>
        <?php if ( count($o_relationships) > 0 ) {
          foreach ($o_relationships as $result) { ?>
            <tr><td><?php print $node->uniquename; ?></td><td><?php print $result->relationship_type ?></td><td><?php print l($result->object_name, "node/".$result->object_nid); ?></td></tr>
          <?php } //end of foreach?>
        <? } if ( count($s_relationships) > 0 ) {
          foreach ($s_relationships as $result) { ?>
            <tr><td><?php print l($result->subject_name, "node/".$result->subject_nid); ?></td><td><?php print $result->relationship_type ?></td><td><?php print $node->uniquename; ?></td></tr>
          <?php } //end	    of foreach
       } ?>
      </table></div></div>
      <br>
    <?php } ?>
    <br>

  <?php } ?> <!-- End of if not Teaser --!>

<!-- Tripal Stock Ends --!>

      <?php print $content; ?>
    </div>
  </div>
  
  <div class="extra-links">
    <div class="terms terms-inline"><?php print theme('links', $taxonomy, array('class' => 'links term-links')) ?></div>
    <?php print $links; ?>
  </div>

</div></div> <!-- /node-inner, /node -->