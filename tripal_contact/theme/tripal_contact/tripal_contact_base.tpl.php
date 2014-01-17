<?php
$contact = $variables['node']->contact; ?>

<div id="tripal_contact-base-box" class="tripal_contact-info-box tripal-info-box">
  <div class="tripal_contact-info-box-title tripal-info-box-title">Details</div>
  <div class="tripal_contact-info-box-desc tripal-info-box-desc"></div> <?php

  // the $headers array is an array of fields to use as the colum headers. 
  // additional documentation can be found here 
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  // This table for the contact has a vertical header (down the first column)
  // so we do not provide headers here, but specify them in the $rows array below.
  $headers = array();
  
  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row.  Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7 
  $rows = array();

  // Contact Name row
  $rows[] = array(
    array(
      'data' => 'Name',
      'header' => TRUE
    ),
    $contact->name,
  );
  // Contact Type row
  $rows[] = array(
    array(
      'data' => 'Type',
      'header' => TRUE
    ),
    $contact->type_id->name,
  );
  
  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table.  Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(
      'id' => 'tripal_contact-table-base',
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  
  // once we have our table array structure defined, we call Drupal's theme_table()
  // function to generate the table.
  print theme_table($table);
  if (property_exists($contact, 'description')) { ?>
    <div style="text-align: justify"><?php print $contact->description; ?></div> <?php 
  } ?>
</div>
