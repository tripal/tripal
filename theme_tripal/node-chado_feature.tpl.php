
   <?php if ($picture) {
      print $picture;
      $feature = $node->feature;
      $accession = $node->accession;
      $organism = $node->organism;
   }?>

   <div class="node<?php if ($sticky) { print " sticky"; } ?><?php if (!$status) { print " node-unpublished"; } ?>">

   <?php if ($page == 0) { ?><h2 class="nodeTitle"><a href="<?php print $node_url?>"><?php print $title?></a>
	<?php global $base_url;
	if ($sticky) { print '<img src="'.base_path(). drupal_get_path('theme','sanqreal').'/img/sticky.gif" alt="sticky icon" class="sticky" />'; } ?>
	</h2><?php }; ?>

	<?php if (!$teaser): ?>
	   <?php if ($submitted): ?>
      <div class="metanode"><p><?php print t('') .'<span class="author">'. theme('username', $node).'</span>' . t(' - Posted on ') . '<span class="date">'.format_date($node->created, 'custom', "d F Y").'</span>'; ?></p></div>
      <?php endif; ?>
      <!-- theme_tripal_feature_feature_id -->
      <!--<div id="feature_notice"><img src="sites/all/modules/tripal_analysis_blast/images/info-128x128.png"><br><i>Feature information and annotations have moved. See below</i></div>-->
      <div id="feature-view">
         <?php

            if($feature->is_obsolete == 't'){
            drupal_set_message(t('This feature is obsolete and no longer used in analysis, but is here for reference.'));
         }?>
         <table class="tripal_table_vert">
            <tr><th>Name</th><td><?php print $feature->featurename; ?></td></tr>
            <tr><th nowrap>Unique Name</th><td><?php print $feature->uniquename; ?></td></tr>
            <tr><th>Internal ID</th><td><?php print $accession; ?></td></tr>
            <tr><th>Length</th><td><?php print $feature->seqlen ?></td></tr>
            <tr><th>Type</th><td><?php print $feature->cvname; ?></td>
            </tr>
            <tr><th>Organism</th><td>
            		<?php 
                     if ($node->org_nid) {
            				print"<a href=\"". url("node/$node->org_nid") ."\">$organism->genus $organism->species ($organism->common_name)</a>";
            		   } else {
            				print"$organism->genus $organism->species ($organism->common_name)";
            		   }
            		?>
            	</td>
           	</tr>
           	
           	<!-- Add library information which this feature belongs to-->
           	<?php if ($node->lib_additions) { ?>
               <tr><th>Library</th><td>
                  <?php
                     $libraries = $node->lib_additions;
                     foreach ($libraries as $lib_url => $lib_name) {
                        // Check if library exists as a node in drupal
                        if ($lib_url) {
                  ?>
                     <a href="<?php print $lib_url?>"><?php print $lib_name?></a><BR>
                  <?php
                        } else {
                           print $lib_name;
                        }
                     }
                  ?>
               </td></tr>
            <?php } ?>
            <!-- End of library addition -->
            
            <!-- theme_tripal_feature_feature_synonyms -->
            <?php
               $synonyms = $node->synonyms;
               if(count($synonyms) > 0){
            ?>
      			<tr><th>Synonyms</th><td>
                  <?php
                  // iterate through each synonym
                  if (is_array($synonyms)) {
                     foreach ($synonyms as $result){
                        print $result->name."<br>";
                     }
                  } else {
                     print $synonyms;
                  }
                  ?>
               	</td></tr>
            <?php } ?>
      		<!-- End of theme_tripal_feature_feature_synonyms -->
         </table>
      </div>
      <!-- End of theme_tripal_feature_feature_id -->

   <?php endif; ?>

   <div class="content">
   <?php if (!$teaser): ?>
     <!-- Control link for the expandableBoxes -->
       <br><a id="tripal_expandableBox_toggle_button" onClick="toggleExpandableBoxes()">[-] Collapse All</a><br><br>
     <!-- End of Control link for the expandableBoxes -->
     <!-- Start of sequences -->
      <div id="feature-sequence" class="tripal_feature-info-box">
      <div class="tripal_expandableBox"><h3>Sequence</h3></div>
      <div class="tripal_expandableBoxContent">
        <?php print ucfirst($feature->cvname); ?> sequence
        <pre><?php print ereg_replace("(.{100})","\\1<br>",$feature->residues); ?></pre>
        <?php
        if(count($orelationships) > 0){
           foreach ($orelationships as $result){
              print "<br>" . ucfirst($result->subject_type) . " sequence";
              print "<pre>" . ereg_replace("(.{100})","\\1<br>",$result->subject_residues) . "</pre>";
           }
        }
        ?>
      </div></div>
     <!-- End of sequences -->
     <!-- Start of theme_tripal_feature_feature_references -->
      <?php
         $references = $node->references;
         if(count($references) > 0){
      ?>
      <div id="feature-references" class="tripal_feature-info-box">
      <div class="tripal_expandableBox"><h3>References</h3></div>
      <div class="tripal_expandableBoxContent">
      <table>
         <tr>
            <th>Dababase</th>
            <th>Accession</th>
         </tr>
      <?php
         foreach ($references as $result){
      ?>
         <tr>
            <td><?php print $result->db_name?></td>
            <td><?php if($result->urlprefix){ ?><a href="<?php print $result->urlprefix.$result->accession?>"><?php print $result->accession?></a><?php } else { print $result->accession; } ?></td>
         </tr>
      <?php  } ?>
         </table></div></div>
      <?php } ?>
     <!-- End of theme_tripal_feature_feature_references -->
     <!-- Start of theme_tripal_feature_feature_relationships -->
       <?php
         $orelationships = $node->orelationships;
        if(count($orelationships) > 0 or count($srelationships) > 0){
            print "<div id=\"feature-relationships\" class=\"tripal_feature-info-box\">";
            print "<div class=\"tripal_expandableBox\"><h3>Relationships</h3></div>";
            print "<div class=\"tripal_expandableBoxContent\">";
      
            if(count($orelationships) > 0){
               foreach ($orelationships as $result){
                  if(isset($result->subject_nid)){
                     print "<a href=\"" . url("node/$result->subject_nid") . "\">$result->subject_name ($result->subject_type)</a> ";
                  } else {
                     print "$result->subject_name ($result->subject_type) ";
                  }
                  print "<b>$result->rel_type</b> ";
                  if(isset($result->object_nid)){
                     print "<a href=\"" . url("node/$result->object_nid") . "\">$result->object_name ($result->object_type)</a> ";
                  } else {
                     print "$result->object_name ($result->object_type) ";
                  }
                  print"<br>";
               }
            }
            if(count($srelationships) > 0){
              foreach ($srelationships as $result){
                  if(isset($result->subject_nid)){
                     print "<a href=\"" . url("node/$result->subject_nid") . "\">$result->subject_name ($result->subject_type)</a> ";
                  } else {
                     print "$result->subject_name ($result->subject_type) ";
                  }
                  print "<b>$result->rel_type</b> ";
                  if(isset($result->object_nid)){
                     print "<a href=\"" . url("node/$result->object_nid") . "\">$result->object_name ($result->object_type)</a> ";
                  } else {
                     print "$result->object_name ($result->object_type) ";
                  }
                  print"<br>";
               }
            }
            print "</div></div>";
         } 
       ?>
     <!-- End of theme_tripal_feature_feature_relationships -->
   <?php endif; ?>
   <?php print $content?>
   </div>
   
   <?php if (!$teaser): ?>
   <?php if ($links) { ?><div class="links"><?php print $links?></div><?php }; ?>
   <?php endif; ?>

   <?php if ($teaser): ?>
   <?php if ($links) { ?><div class="linksteaser"><div class="links"><?php print $links?></div></div><?php }; ?>
   <?php endif; ?>

   <?php if (!$teaser): ?>
   <?php if ($terms) { ?><div class="taxonomy"><span><?php print t('tags') ?></span> <?php print $terms?></div><?php } ?>
   <?php endif; ?>

   </div>
