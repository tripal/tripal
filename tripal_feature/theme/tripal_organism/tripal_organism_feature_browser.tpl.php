<?php

$organism = $variables['node']->organism;
$features = $organism->feature_browser['features'];
$pager    = $organism->feature_browser['pager'];
$enabled  = $organism->feature_browser['enabled'];

// only show this block if it is enabled
if ($enabled) { 
  if (count($features) > 0) { ?>
    <div id="tripal_organism-feature_browser-box" class="tripal_organism-info-box tripal-info-box">
      <div class="tripal_organism-info-box-title tripal-info-box-title">Feature Browser</div>
      <div class="tripal_organism-info-box-desc tripal-info-box-desc">The following browser provides a quick view for new visitors.  Use the searching mechanism to find specific features.</div> <?php
      // let admins know they can customize the terms that appear in the list
      if (user_access('access administration pages')) { ?>
         <div class="tripal-no-results">Administrators, to disable this browser and 
         remove it from the list of resources, navigate to the <a href="<?php print url('admin/tripal/tripal_feature/configuration') ?>">Tripal feature configuration page</a> 
         and hide the "Feature Browser".
         </div><?php 
      }?>
      <table id="tripal_organism-table-feature_browser" class="tripal_organism-table tripal-table tripal-table-horz">     
        <tr class="tripal_organism-table-odd-row tripal-table-even-row">
          <th>Feature Name</th>
          <th>Unique Name</th>
          <th>Type</th>
        </tr> <?php
        foreach ($features as $feature){ 
          $class = 'tripal_organism-table-odd-row tripal-table-odd-row';
          if ($i % 2 == 0 ) {
            $class = 'tripal_organism-table-odd-row tripal-table-even-row';
          } ?>
          <tr class="<?php print $class ?>">
            <td><?php 
              if ($feature->nid) {    
                $link =   url("node/$feature->nid");        
                print "<a href=\"$link\">$feature->name</a>";
              } else {
                print $feature->name;
              }?>
            </td>
            <td><?php print $feature->uniquename?></td>
            <td><?php print $feature->type_name?></td>
          </tr><?php
          $i++;  
        } ?>
      </table>
      <?php print $pager ?>
    </div> <?php
  } 
  else {
    // if there are no results and this is the admin user then show some instructions
    // otherwise nothing is shown.
    if(user_access('access administration pages')){ ?>
      <div id="tripal_organism-feature_browser-box" class="tripal_organism-info-box tripal-info-box">
        <div class="tripal_organism-info-box-title tripal-info-box-title">Feature Browser</div>
        <div class="tripal-no-results">
          There are no features available for browsing
          <p><br>Administrators, perform the following to show features in this browser:
          <ul>
            <li>Load features for this organism using the <a href="<?php print url('admin/tripal/tripal_feature/fasta_loader');?>">FASTA loader</a>, <a href="<?php print url('admin/tripal/tripal_feature/gff3_load');?>">GFF loader</a> or <a href="<?php print url('admin/tripal/tripal_bulk_loader_template');?>">Bulk Loader</a>.</li>
            <li>Sync the features that should have pages using the <a href="<?php print url('admin/tripal/tripal_feature/sync');?>">Sync Features</a> tool.</li>
            <li>Return to this page to browse features.</li>
            <li>Ensure the user <a href="<?php print url('admin/user/permissions'); ?>"> has permission</a> to view the feature content</li>
          </ul> 
          <br><br>To disable this browser and remove it from the list of resources:
          <ul>
            <li>Navigate to the <a href="<?php print url('admin/tripal/tripal_feature/configuration') ?>">Tripal feature configuration page</a> and hide the "Feature Browser"</li>
          </ul>
          </p>
          This page will not appear to site visitors unless features are present.
        </div>         
      </div><?php
    }
  }
}



