<?php
$project  = $variables['node']->project;

// expand project to include pubs 
$project = tripal_core_expand_chado_vars($project, 'table', 'project_pub', array('return_array' => 1));
$project_pubs = $project->project_pub;

if (count($project_pubs) > 0) { ?>
  <div id="tripal_project-pub-box" class="tripal_project-info-box tripal-info-box">
    <div class="tripal_project-info-box-title tripal-info-box-title">Publications</div>
    <div class="tripal_project-info-box-desc tripal-info-box-desc"></div>
  
    <table id="tripal_project-pub-table" class="tripal_project-table tripal-table tripal-table-vert" style="border-bottom:solid 2px #999999">
      <tr>
        <th>Year</th>
        <th>Publication</th>
      </tr> <?php
      $i = 0;
      foreach ($project_pubs AS $project_pubs) {
        $pub = $project_pubs->pub_id;
        $class = 'tripal_project-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_project-table-odd-row tripal-table-even-row';
        }
        $pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.title'); 
        $citation = $pub->title;  // use the title as the default citation
        
        // get the citation for this pub if it exists
        $values = array(
          'pub_id' => $pub->pub_id, 
          'type_id' => array(
            'name' => 'Citation',
          ),
        );

        $options = array('return_array' => 1);
        $citation_prop = tripal_core_generate_chado_var('pubprop', $values, $options); 
        if (count($citation_prop) == 1) {
          $citation_prop = tripal_core_expand_chado_vars($citation_prop, 'field', 'pubprop.value');
          $citation = $citation_prop[0]->value;
        }
        
        // if the publication is synced then link to it
        if ($pub->nid) {
          // replace the title with a link
          $link = l($pub->title, 'node/' . $pub->nid ,array('attributes' => array('target' => '_blank')));
          $citation = preg_replace('/' . $pub->title . '/', $link, $citation);
        }
        ?>
        <tr class="<?php print $class ?>">
          <td><?php print $pub->pyear ?></td>
          <td><?php print $citation ?></td>
        </tr><?php 
        $i++;
      }  ?>
    </table>
  </div><?php 
}
