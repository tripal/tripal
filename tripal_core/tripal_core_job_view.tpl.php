<?php
$job = $variables['job'];

$cancel_link = '';
$return_link = url("admin/tripal/tripal_jobs/");
if($job->start_time == 0 and $job->end_time == 0){
   $cancel_link = url("admin/tripal/tripal_jobs/cancel/".$job->job_id);
}
$rerun_link = url("admin/tripal/tripal_jobs/rerun/".$job->job_id);
 
?>

<div id="tripal_core-job_view-box" class="tripal_core-info-box tripal-info-box">
  <div class="tripal_core-info-box-title tripal-info-box-title">Details for Job <?php print $job->job_id?></div>
  <div class="tripal_core-info-box-desc tripal-info-box-desc"></div>
   <a href="<?php print $return_link?>">Return to jobs list</a> | 
   <a href="<?php print $rerun_link?>">Re-run this job</a> | 
   <a href="<?php print $cancel_link?>">Cancel this Job</a><br>
   <table id="tripal_core-job_view-table" class="tripal_core-table tripal-table tripal-table-vert">
      <tr class="tripal_core-table-odd-row tripal-table-even-row tripal-table-first-row">
         <th>Job Description</th>
         <td><?php print $job->job_name?></td>
      </tr>
      <tr class="tripal_core-table-odd-row tripal-table-odd-row">
         <th>Submitting Module</th>
         <td><?php print $job->modulename?></td>
      </tr>
      <tr class="tripal_core-table-odd-row tripal-table-even-row">
         <th>Callback function</th>
         <td><?php print $job->callback?></td>
      </tr>
      <tr class="tripal_core-table-odd-row tripal-table-odd-row">
         <th>Arguments</th>
         <td>
           <table>
           <?php foreach($job->arguments as $key => $value){
              print "<tr><td>$key</td><td>$value</td></tr>";
           } ?>
           </table>
         </td>
      </tr>
      <tr class="tripal_core-table-odd-row tripal-table-even-row">
         <th>Progress</th>
         <td><?php print $job->progress?>%</td>
      </tr>
      <tr class="tripal_core-table-odd-row tripal-table-odd-row">
         <th>Status</th>
         <td><?php print $job->job_status?></td>
      </tr>
      <tr class="tripal_core-table-odd-row tripal-table-even-row">
         <th>Process ID</th>
         <td><?php print $job->pid?></td>
      </tr>
      <tr class="tripal_core-table-odd-row tripal-table-odd-row">
         <th>Submit Date</th>
         <td><?php print $job->submit_date?></td>
      </tr>
      <tr class="tripal_core-table-odd-row tripal-table-even-row">
         <th>Start time</th>
         <td><?php print $job->start_time?></td>
      </tr>
      <tr class="tripal_core-table-odd-row tripal-table-odd-row">
         <th>End time</th>
         <td><?php print $job->end_time?></td>
      </tr>
      <tr class="tripal_core-table-odd-row tripal-table-even-row">
         <th>Error Message</th>
         <td><?php print $job->error_msg?></td>
      </tr>
      <tr class="tripal_core-table-odd-row tripal-table-odd-row">
         <th>Priority</th>
         <td><?php print $job->priority?></td>
      </tr>
      <tr class="tripal_core-table-odd-row tripal-table-even-row tripal-table-last-row">
         <th>Submitting User</th>
         <td><?php print $job->username?></td>
      </tr>
   </table>
</div>


