<?php
$contact  = $variables['node']->contact;

// expand contact to include pubs 
$options = array('return_array' => 1);
$contact = tripal_core_expand_chado_vars($contact, 'table', 'pubauthor_contact', $options);

$pubauthor_contacts = $contact->pubauthor_contact;
?>

<div id="tripal_pubauthor_contact-pub-box" class="tripal_pubauthor_contact-info-box tripal-info-box">
  <div class="tripal_pubauthor_contact-info-box-title tripal-info-box-title">Publications</div>
  <div class="tripal_pubauthor_contact-info-box-desc tripal-info-box-desc"></div>

  <table id="tripal_pubauthor_contact-pub-table" class="tripal_pubauthor_contact-table tripal-table tripal-table-vert" style="border-bottom:solid 2px #999999">
    <tr>
      <th>Year</th>
      <th>Publication</th></tr> <?php
      $i = 0;
      foreach ($pubauthor_contacts AS $pubauthor_contact) {
        $pub = $pubauthor_contact->pubauthor_id->pub_id;
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
        
        // if the publicatio is synced then link to it
        if ($pub->nid) {
          // replace the title with a link
          $link = l($pub->title, 'node/' . $pub->nid ,array('attributes' => array('target' => '_blank')));
          $citation = preg_replace('/' . $pub->title . '/', $link, $citation);
        }
          
        
        $class = 'tripal_pubauthor_contact-table-odd-row tripal-table-odd-row';
        if($i % 2 == 0 ){
           $class = 'tripal_pubauthor_contact-table-odd-row tripal-table-even-row';
        } ?>
        <tr class="<?php print $class ?>">
          <td><?php print $pub->pyear ?></td>
          <td><?php print $citation ?></td>
        </tr><?php 
        $i++;
      }  ?>
  </table>
</div>
