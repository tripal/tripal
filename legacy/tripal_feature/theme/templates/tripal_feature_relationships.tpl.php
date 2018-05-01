<?php
/* Typically in a Tripal template, the data needed is retrieved using a call to
 * chado_expand_var function.  For example, to retrieve all
 * of the feature relationships for this node, the following function call would be made:
 *
 *   $feature = chado_expand_var($feature,'table','feature_relationship');
 *
 * However, this function call can be extremely slow when there are numerous relationships.
 * This is because the chado_expand_var function is recursive and expands
 * all data following the foreign key relationships tree.  Therefore, to speed retrieval
 * of data, a special variable is provided to this template:
 *
 *   $feature->all_relationships;
 *
 * This variable is an array with two sub arrays with the keys 'object' and 'subject'.  The array with
 * key 'object' contains relationships where the feature is the object, and the array with
 * the key 'subject' contains relationships where the feature is the subject
 */
$feature = $variables['node']->feature;

$all_relationships = $feature->all_relationships;
$object_rels = $all_relationships['object'];
$subject_rels = $all_relationships['subject'];

if (count($object_rels) > 0 or count($subject_rels) > 0) { ?>
    <div class="tripal_feature-data-block-desc tripal-data-block-desc"></div> <?php

  // first add in the subject relationships.
  foreach ($subject_rels as $rel_type => $rels) {
    foreach ($rels as $obj_type => $objects) {

      // Make the verb in the sentence make sense in English.
      switch ($rel_type) {
        case 'integral part of':
        case 'instance of':
          $verb = 'is an';
          break;
        case 'proper part of':
        case 'transformation of':
        case 'genome of':
        case 'part of':
        case 'position of':
        case 'sequence of':
        case 'variant of':
          $verb = 'is a';
          break;
        case 'derives from':
        case 'connects on':
        case 'contains':
        case 'finishes':
        case 'guides':
        case 'has origin':
        case 'has part':
        case 'has quality':
        case 'is consecutive sequence of':
        case 'maximally overlaps':
        case 'overlaps':
        case 'starts':
          $verb = '';
          break;
        default:
          $verb = 'is';
      } ?>

        <p>
            This <?php print $feature->type_id->name; ?> <?php print $verb ?> <?php print $rel_type ?>
            the following <b><?php print $obj_type ?></b> feature(s): <?php

          // the $headers array is an array of fields to use as the colum headers.
          // additional documentation can be found here
          // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
          $headers = ['Feature Name', 'Unique Name', 'Species', 'Type'];

          // the $rows array contains an array of rows where each row is an array
          // of values for each column of the table in that row.  Additional documentation
          // can be found here:
          // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
          $rows = [];

          foreach ($objects as $object) {
            // link the feature to it's node
            $feature_name = $object->record->object_id->name;
            if (property_exists($object->record, 'nid')) {
              $feature_name = l($feature_name, "node/" . $object->record->nid, ['attributes' => ['target' => "_blank"]]);
            }
            // link the organism to it's node
            $organism = $object->record->object_id->organism_id;
            $organism_name = $organism->genus . " " . $organism->species;
            if (property_exists($organism, 'nid')) {
              $organism_name = l("<i>" . $organism->genus . " " . $organism->species . "</i>", "node/" . $organism->nid, ['html' => TRUE]);
            }
            $rows[] = [
              ['data' => $feature_name, 'width' => '30%'],
              [
                'data' => $object->record->object_id->uniquename,
                'width' => '30%',
              ],
              ['data' => $organism_name, 'width' => '30%'],
              [
                'data' => $object->record->object_id->type_id->name,
                'width' => '10%',
              ],
            ];
          }
          // the $table array contains the headers and rows array as well as other
          // options for controlling the display of the table.  Additional
          // documentation can be found here:
          // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
          $table = [
            'header' => $headers,
            'rows' => $rows,
            'attributes' => [
              'id' => 'tripal_feature-table-relationship-object',
              'class' => 'tripal-data-table',
            ],
            'sticky' => FALSE,
            'caption' => '',
            'colgroups' => [],
            'empty' => '',
          ];

          // once we have our table array structure defined, we call Drupal's theme_table()
          // function to generate the table.
          print theme_table($table); ?>
        </p>
        <br><?php
    }
  }

  // second add in the object relationships.
  foreach ($object_rels as $rel_type => $rels) {
    foreach ($rels as $subject_type => $subjects) {

      // Make the verb in the sentence make sense in English.
      switch ($rel_type) {
        case 'integral part of':
        case 'instance of':
          $verb = 'are an';
          break;
        case 'proper part of':
        case 'transformation of':
        case 'genome of':
        case 'part of':
        case 'position of':
        case 'sequence of':
        case 'variant of':
          $verb = 'are a';
          break;
        case 'derives from':
        case 'connects on':
        case 'contains':
        case 'finishes':
        case 'guides':
        case 'has origin':
        case 'has part':
        case 'has quality':
        case 'is consecutive sequence of':
        case 'maximally overlaps':
        case 'overlaps':
        case 'starts':
          $verb = '';
          break;
        default:
          $verb = 'are';
      } ?>
        <p>The following
            <b><?php print $subjects[0]->record->subject_id->type_id->name ?></b>
            feature(s) <?php print $verb ?> <?php print $rel_type ?>
            this <?php print $feature->type_id->name; ?>: <?php
          // the $headers array is an array of fields to use as the colum headers.
          // additional documentation can be found here
          // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
          $headers = ['Feature Name', 'Unique Name', 'Species', 'Type'];

          // the $rows array contains an array of rows where each row is an array
          // of values for each column of the table in that row.  Additional documentation
          // can be found here:
          // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
          $rows = [];

          foreach ($subjects as $subject) {
            // link the feature to it's node
            $feature_name = $subject->record->subject_id->name;
            if (property_exists($subject->record, 'nid')) {
              $feature_name = l($feature_name, "node/" . $subject->record->nid, ['attributes' => ['target' => "_blank"]]);
            }
            // link the organism to it's node
            $organism = $subject->record->subject_id->organism_id;
            $organism_name = $organism->genus . " " . $organism->species;
            if (property_exists($organism, 'nid')) {
              $organism_name = l("<i>" . $organism->genus . " " . $organism->species . "</i>", "node/" . $organism->nid, ['html' => TRUE]);
            }
            $rows[] = [
              ['data' => $feature_name, 'width' => '30%'],
              [
                'data' => $subject->record->subject_id->uniquename,
                'width' => '30%',
              ],
              ['data' => $organism_name, 'width' => '30%'],
              [
                'data' => $subject->record->subject_id->type_id->name,
                'width' => '10%',
              ],
            ];
          }
          // the $table array contains the headers and rows array as well as other
          // options for controlling the display of the table.  Additional
          // documentation can be found here:
          // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
          $table = [
            'header' => $headers,
            'rows' => $rows,
            'attributes' => [
              'id' => 'tripal_feature-table-relationship-subject',
              'class' => 'tripal-data-table',
            ],
            'sticky' => FALSE,
            'caption' => '',
            'colgroups' => [],
            'empty' => '',
          ];

          // once we have our table array structure defined, we call Drupal's theme_table()
          // function to generate the table.
          print theme_table($table); ?>
        </p>
        <br><?php
    }
  }
}
