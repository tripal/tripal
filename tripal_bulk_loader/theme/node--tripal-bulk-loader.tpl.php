<div id="tripal_bulk_loader-base-box" class="tripal_bulk_loader-info-box tripal-info-box">
  <div class="tripal_bulk_loader-info-box-title tripal-info-box-title">
    <?php if ($teaser) { print l($node->title, 'node/'.$node->nid); } ?>
  </div>
  <div class="tripal_bulk_loader-data-block-desc tripal-data-block-desc"></div>

  <table id="tripal_bulk_loader-base-table" class="tripal_bulk_loader-table tripal-table tripal-table-vert">
    <tr class="tripal_bulk_loader-table-odd-row tripal-table-odd-row">
      <th>Job Name</th>
      <td><?php print $node->loader_name;?></td>
    </tr>
    <tr class="tripal_bulk_loader-table-even-row tripal-table-even-row">
      <th>Submitted By</th>
      <td><span class="author"><?php //print theme('username', $node); ?></span></td>
    </tr>
    <tr class="tripal_bulk_loader-table-odd-row tripal-table-odd-row">
      <th>Job Creation Date</th>
      <td><?php print format_date($node->created, 'custom', "F j, Y, g:i a"); ?></td>
    </tr>
    <tr class="tripal_bulk_loader-table-even-row tripal-table-even-row">
      <th>Last Updated</th>
      <td><?php print format_date($node->changed, 'custom', "F j, Y, g:i a"); ?></td>
    </tr>
    <tr class="tripal_bulk_loader-table-odd-row tripal-table-odd-row">
      <th>Template Name</th>
      <td><?php print $node->template->name; ?></td>
    </tr>
    <tr class="tripal_bulk_loader-table-even-row tripal-table-even-row">
      <th>Data File</th>
      <td><?php print $node->file;?></td>
    </tr>
    <tr class="tripal_bulk_loader-table-odd-row tripal-table-odd-row">
      <th>Job Status</th>
      <td><?php print $node->job_status;?></td>
    </tr>
    <?php if (isset($node->job)) { if (isset($node->job->progress)) { ?>
    <tr class="tripal_bulk_loader-table-even-row tripal-table-even-row">
      <th>Job Progress</th>
      <td><?php print $node->job->progress . '% (' . l('view job', 'admin/tripal/tripal_jobs/view/' . $node->job_id) . ')';?></td>
    </tr>
    <?php }} ?>
  </table>
</div>

<?php if (!$teaser) { ?>
<?php print drupal_render(drupal_get_form('tripal_bulk_loader_add_loader_job_form', $node)); ?>

  <?php if (!empty($node->inserted_records)) {
    print '<h3>Loading Summary</h3>';
    $rows = array();
    $total = 0;
    foreach ($node->inserted_records as $r) {
      $row = array();
      $row[] = $r->table_inserted_into;
      $row[] = $r->num_inserted;
      $rows[] = $row;
      $total = $total + $r->num_inserted;
    }
    $rows[] = array('<b>TOTAL</b>','<b>'.$total.'</b>');
    print theme('table',array('header'=>array('Chado Table', 'Number of Records Inserted'), 'rows'=>$rows));
  } ?>
  <br>

  <?php print drupal_render(drupal_get_form('tripal_bulk_loader_set_constants_form', $node)); ?>

<?php print theme('tripal_bulk_loader_template', $node->template->template_id); ?>
<?php } ?>