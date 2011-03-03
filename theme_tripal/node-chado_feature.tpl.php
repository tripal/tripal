
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
        <?php print $feature->cvname; ?> sequence        
        <pre id="tripal_feature-sequence"><?php 
           if($feature->residues){
              print ereg_replace("(.{100})","\\1<br>",$feature->residues); 
           } else {
              print "sequence currently not available";
           }
        ?></pre>
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
      <table class="tripal_feature-references-table">
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
     <!-- Start of theme_tripal_feature_feature_featurelocs -->
      <?php
         $featurelocs = $node->featurelocs;
         if(count($featurelocs) > 0){
      ?>
      <div id="feature-featurelocs" class="tripal_feature-info-box">
      <div class="tripal_expandableBox"><h3><?php print $feature->featurename;?> is Located on These Features</h3></div>
      <div class="tripal_expandableBoxContent">
      <table class="tripal_feature-locations-table">
         <tr>
            <th>Name</th>  
            <th>Type</th>
            <th>Position</th>
            <th>Phase</th>
            <th>Strand</th>
         </tr>
         <?php foreach ($featurelocs as $index => $loc){ 
            $locname = $loc->src_name;
            if($loc->nid){
              $locname = "<a href=\"" . url("node/$loc->nid") . "\">$loc->src_name</a> ";
            }
         ?>
            <tr>
               <td><?php print $locname ?></td>
               <td><?php print $loc->cvname ?></td>
               <td><?php print $loc->src_name .":".$loc->fmin . ".." . $loc->fmax ?></td>
               <td><?php print $loc->phase ?></td>
               <td><?php print $loc->strand ?></td>
            </tr>
         <?php  } ?>
         </table></div></div>
      <?php } ?>
     <!-- End of theme_tripal_feature_feature_references -->
     <!-- Start of theme_tripal_feature_feature_myfeaturelocs -->
      <?php
         $myfeaturelocs = $node->myfeaturelocs;
         if(count($myfeaturelocs) > 0){
      ?>
      <div id="feature-myfeaturelocs" class="tripal_feature-info-box">
      <div class="tripal_expandableBox"><h3>Features Located on <?php print $feature->featurename;?></h3></div>
      <div class="tripal_expandableBoxContent">
      <table class="tripal_feature-locations-table">
         <tr>
            <th>Name</th>
            <th>Type</th>
            <th>Position</th>
            <th>Phase</th>
            <th>Strand</th>
         </tr>
         <?php foreach ($myfeaturelocs as $index => $loc){ 
            $locname = $loc->name;
            if($loc->nid){
              $locname = "<a href=\"" . url("node/$loc->nid") . "\">$loc->name</a> ";
            }
         ?>
            <tr>
               <td><?php print $locname ?></td>
               <td><?php print $loc->cvname ?></td>
               <td><?php print $loc->src_name .":".$loc->fmin . ".." . $loc->fmax ?></td>
               <td><?php print $loc->phase ?></td>
               <td><?php print $loc->strand ?></td>
            </tr>
         <?php  } ?>
         </table></div></div>
      <?php } ?>
     <!-- End of theme_tripal_feature_feature_references -->
     <!-- Start of theme_tripal_feature_feature_relationships -->
       <?php
         print "<div id=\"feature-srelationships\" class=\"tripal_feature-info-box\">";
         print "<div class=\"tripal_expandableBox\"><h3>Parent Relationships</h3></div>";
         print "<div class=\"tripal_expandableBoxContent\">";
         $srelationships = $node->subject_relationships;
         if(count($srelationships) > 0){
            print "
            <table class=\"tripal_feature-relationships-subject-table\">
               <tr>
                  <th>Relationship</th>
                  <th>Feature</th>
                  <th>Type</th>
               </tr>
            ";      
            foreach ($srelationships as $result){
               print "<tr>";
               print "<td><b>$result->rel_type</b></td>";
               print "<td>";
               if(isset($result->object_nid)){
                  print "<a href=\"" . url("node/$result->object_nid") . "\">$result->object_name</a> ";
               } else {
                  print "$result->object_name ";
               }
               print "</td>";
               print "<td>$result->object_type</td>";
            }
            print "</table>";
         } 
         print "</div></div>";
       ?>
     <!-- End of theme_tripal_feature_feature_relationships -->
     <!-- Start of theme_tripal_feature_feature_relationships -->
       <?php
         print "<div id=\"feature-srelationships\" class=\"tripal_feature-info-box\">";
         print "<div class=\"tripal_expandableBox\"><h3>Child Relationships</h3></div>";
         print "<div class=\"tripal_expandableBoxContent\">";
         $orelationships = $node->object_relationships;
         if(count($orelationships) > 0){    
            print "
            <table class=\"tripal_feature-relationships-object-table\">
               <tr>
                  <th>Name</th>
                  <th>Type</th>
                  <th>Relationship</th>
                  <th>Position</th>
               </tr>
            ";
            foreach ($orelationships as $result){
               $subject_name = $result->subject_name;
               if(!$subject_name){
                  $subject_name = $result->subject_uniquename;
               }
               print "<tr>";
               print "<td>";
               if(isset($result->subject_nid)){
                  print "<a href=\"" . url("node/$result->subject_nid") . "\">$result->subject_name ($result->subject_type)</a> ";
               } else {
                  print "$subject_name";
               }          
               print "</td>";
               print "<td>$result->subject_type</td>";
               print "<td><b>$result->rel_type</b></td>";
               print "<td>";
               $featurelocs = $result->featurelocs;
               if($featurelocs){
                  foreach($featurelocs as $src => $attrs){
                     print "$attrs->src_name ($attrs->src_cvname):$attrs->fmin $attrs->fmax</br>";
                  } 
               }
               print "</td>";
               print "</tr>";
            }
            print "</table>";           

           print "</div></div>";
         } 
       ?>

     <!-- End of theme_tripal_feature_feature_relationships -->
     <!-- Start of theme_tripal_feature_feature_floc_sequences -->
      <?php
         $floc_sequences = $node->floc_sequences;
         foreach($floc_sequences as $seq => $attrs){
           print "<div id=\"feature-floc-squences\" class=\"tripal_feature-info-box\">";
           print "<div class=\"tripal_expandableBox\"><h3>".$attrs['type']." Sequence </h3></div>";
           print "<div class=\"tripal_expandableBoxContent\">";
           print $attrs['formatted_seq'];
           print "</div></div>";          
         } 
       ?>
     <!-- End of theme_tripal_feature_feature_floc_sequences -->
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
