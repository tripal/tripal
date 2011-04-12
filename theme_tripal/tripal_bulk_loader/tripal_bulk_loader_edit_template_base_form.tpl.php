
<div id="tripal-bulk-loader-fields">

<?php print drupal_render($form['template_name']); ?>

<!-- For each field display details plus edit/delete buttons-->
<?php if ($form['fields']['total_fields']['#value'] > 0) {?>
<fieldset><legend>Current Fields</legend>
  <table>
    <tr>
      <th>Field Name</th>
      <th> Chado Table</th>
      <th>Chado Field</th>
      <th>Worksheet</th>
      <th>Column</th>
      <th>Constant Value</th>
      <th></th>
    </tr>
  <?php for($i=1; $i<$form['fields']['total_fields']['#value']; $i++) { ?>
    <tr>
      <td><?php print drupal_render($form['fields']["field_name-$i"]);?></td>
      <td><?php print drupal_render($form['fields']["chado_table_name-$i"]);?></td>
      <td><?php print drupal_render($form['fields']["chado_field_name-$i"]);?></td>
      <td><?php print drupal_render($form['fields']["sheet_name-$i"]);?></td>
      <td><?php print drupal_render($form['fields']["column_num-$i"]);?></td>
      <td><?php print drupal_render($form['fields']["constant_value-$i"]);?></td>
      <td>
        <?php print drupal_render($form['fields']["edit-$i"]);?>
        <?php print drupal_render($form['fields']["delete-$i"]);?>
        <?php print drupal_render($form['fields']["field_index-$i"]);?>
      </td>
    </tr>
  <?php } ?>
  </table>
<?php 
  } 
  unset($form['fields']);
?>
</fieldset>
<!-- Display Rest of form -->
<?php print drupal_render($form); ?>
</div>