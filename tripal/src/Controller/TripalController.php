<?php

namespace Drupal\tripal\Controller;

use Drupal\Core\Breadcrumb\Breadcrumb;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Controller routines for the Tripal Module
 */
class TripalController extends ControllerBase{

  /**
   * Constructs the TripalController.
   *
   */
  public function __construct() {
  }

  public function tripalStorage() {
    //
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalExtensions() {
    //
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalJobs() {

    // set the breadcrumb
    $breadcrumb = new Breadcrumb();
    $breadcrumb->addLink(Link::fromTextAndUrl('Home',
        Url::fromRoute('<front>')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Administration',
        Url::fromUri('internal:/admin')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Tripal',
        Url::fromUri('internal:/admin/tripal')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Jobs',
        Url::fromUri('internal:/admin/tripal/tripal_jobs')));


    $view = \Drupal\views\Views::getView('tripal_jobs');
    $view->setDisplay('default');
    if ($view->access('default')) {
      return $view->render();
    }
    else {
      return [
        '#markup' => 'You do not have access to view this page.',
      ];
    }
  }

  public function tripalJobsCancel($id) {
    //tripal_cancel_job in tripal.jobs.api.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalJobsStatus($id) {
    //tripal_jobs_status_view in tripal.jobs.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalJobsRerun($id) {
    //tripal_rerun_job in tripal.jobs.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalJobsView($id) {

    // set the breadcrumb
    $breadcrumb = new Breadcrumb();
    $breadcrumb->addLink(Link::fromTextAndUrl('Home',
        Url::fromRoute('<front>')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Administration',
        Url::fromUri('internal:/admin')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Tripal',
        Url::fromUri('internal:/admin/tripal')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Jobs',
        Url::fromUri('internal:/admin/tripal/tripal_jobs')));

    // get the job record
    $sql =
    "SELECT TJ.job_id,TJ.uid,TJ.job_name,TJ.modulename,TJ.progress,
            TJ.status as job_status, TJ,submit_date,TJ.start_time,
            TJ.end_time,TJ.priority,U.name as username,TJ.arguments,
            TJ.callback,TJ.error_msg,TJ.pid
     FROM {tripal_jobs} TJ
       INNER JOIN users U on TJ.uid = U.uid
     WHERE TJ.job_id = :job_id";
    $results = db_query($sql, [':job_id' => $id]);
    $job = $results->fetchObject();

    // We do not know what the arguments are for and we want to provide a
    // meaningful description to the end-user. So we use a callback function
    // defined in the module that created the job to describe in an array
    // the arguments provided.  If the callback fails then just use the
    // arguments as they are.  Historically, job arguments were separated with
    // two colon. We now store them as a serialized array. So, we need to handle
    // both cases.
    if (preg_match("/::/", $job->arguments)) {
      $args = preg_split("/::/", $job->arguments);
    }
    else {
      $args = unserialize($job->arguments);
    }
    $arg_hook = $job->modulename . "_job_describe_args";
    if (is_callable($arg_hook)) {
      $new_args = call_user_func_array($arg_hook, [$job->callback, $args]);
      if (is_array($new_args) and count($new_args)) {
        $job->arguments = $new_args;
      }
      else {
        $job->arguments = $args;
      }
    }
    else {
      $job->arguments = $args;
    }
    // generate the list of arguments for display
    $arguments = '';
    foreach ($job->arguments as $key => $value) {
      if (is_array($value)) {
        $value = print_r($value, TRUE);
      }
      $arguments .= "$key: $value<br>";
    }

    // build the links
    $items = [];
    $items[] = l('Return to jobs list', "admin/tripal/tripal_jobs/");
    $items[] = l('Re-run this job', "admin/tripal/tripal_jobs/rerun/" . $job->job_id);
    if ($job->start_time == 0 and $job->end_time == 0) {
      $items[] = l('Cancel this job', "admin/tripal/tripal_jobs/cancel/" . $job->job_id);
      $items[] = l('Execute this job', "admin/tripal/tripal_jobs/execute/" . $job->job_id);
    }
    $links = theme_item_list([
      'items' => $items,
      'title' => '',
      'type' => 'ul',
      'attributes' => [
        'class' => ['action-links'],
      ],
    ]);

    // make our start and end times more legible
    $job->submit_date = tripal_get_job_submit_date($job);
    $job->start_time = tripal_get_job_start($job);
    $job->end_time = tripal_get_job_end($job);

    // construct the table headers
    $header = ['Detail', 'Value'];

    // construct the table rows
    $rows = [];
    $rows[] = ['Job Description', $job->job_name];
    $rows[] = ['Submitting Module', $job->modulename];
    $rows[] = ['Callback function', $job->callback];
    $rows[] = ['Arguments', $arguments];
    $rows[] = ['Progress', $job->progress . "%"];
    $rows[] = ['Status', $job->job_status];
    $rows[] = ['Process ID', $job->pid];
    $rows[] = ['Submit Date', $job->submit_date];
    $rows[] = ['Start time', $job->start_time];
    $rows[] = ['End time', $job->end_time];
    $rows[] = ['Priority', $job->priority];
    $rows[] = ['Submitting User', $job->username];

    $table = [
      'header' => $header,
      'rows' => $rows,
      'attributes' => ['class' => 'tripal-data-table'],
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => [],
      'empty' => '',
    ];

    $content['links'] = [
      '#type' => 'markup',
      '#markup' => $links,
    ];
    $content['job_title'] = [
      '#type' => 'item',
      '#title' => t('Job Title'),
      '#markup' => $job->job_name,
    ];
    $content['job_status'] = [
      '#type' => 'item',
      '#title' => t('Status'),
      '#markup' => $job->job_status,
    ];
    $content['details_fset'] = [
      '#type' => 'fieldset',
      '#title' => t('Job Details'),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#attributes' => [
        'class' => ['collapsible'],
      ],
      '#attached' => [
        'js' => ['misc/collapse.js', 'misc/form.js'],
      ],
    ];
    $content['details_fset']['job_details'] = [
      '#type' => 'markup',
      '#markup' => theme_table($table),
    ];
    $content['log_fset'] = [
      '#type' => 'fieldset',
      '#title' => t('Job Logs'),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#attributes' => [
        'class' => ['collapsible'],
      ],
      '#attached' => [
        'js' => ['misc/collapse.js', 'misc/form.js'],
      ],
    ];
    $content['log_fset']['job_logs'] = [
      '#type' => 'markup',
      '#markup' => '<pre class="tripal-job-logs">' . $job->error_msg . '</pre>',
    ];
    return $content;
  }

  public function tripalJobsEnable() {
    // tripal_enable_view in tripal.jobs.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalJobsExecute($id) {
    //tripal_jobs_view in tripal.jobs.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalAttachField($id) {
    //tripal_jobs_view in tripal.jobs.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalDisableNotification($id) {
    //tripal_disable_admin_notification in tripal.admin_blocks.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalFieldNotification($field_name_note, $bundle_id, $module, $field_or_instance) {
    //tripal_admin_notification_import_field in tripal.admin_blocks.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalCvLookup() {
    //tripal_vocabulary_lookup_vocab_page in tripal.term_lookup.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalCVTerm($vocabulary, $accession) {
    //tripal_vocabulary_lookup_term_page in tripal.term_lookup.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalCVTermChildren($vocabulary, $accession) {
    //tripal_vocabulary_lookup_term_children_ajax in tripal.term_lookup.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalFileUpload($type, $filename, $action = NULL, $chunk = 0) {
    return tripal_file_upload($type, $filename, $action, $chunk);
  }

  public function tripalDataLoaders() {
    //tripal_file_upload in tripal.upload.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  //@TODO A controller to process the TripalRouters for dataLoaders.

  public function tripalDataCollections() {
    //tripal_user_collections_view_page in tripal.collections.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalContentUnpublishOrphans() {
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  /**
   * Provides contents for the File Usgae page.
   */
  public function tripalFilesUsage() {

    // set the breadcrumb
    $breadcrumb = new Breadcrumb();
    $breadcrumb->addLink(Link::fromTextAndUrl('Home',
      Url::fromRoute('<front>')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Administration',
      Url::fromUri('internal:/admin')));
    $breadcrumb->addLink(Link::fromTextAndUrl('Tripal',
      Url::fromUri('internal:/admin/tripal')));
    $breadcrumb->addLink(Link::fromTextAndUrl('User File Management',
      Url::fromUri('internal:/admin/tripal/files')));

    $content = [
      [
        '#type' => 'markup',
        '#markup' => 'Usage reports coming in the future...',
      ],
    ];

    return $content;
  }

  public function tripalDataCollectionsView() {
    //tripal_admin_file_usage_page in tripal.admin_files.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalUserFiles($user) {
    //tripal_user_files_page in tripal.user.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalUserFileDetails($uid, $file_id) {
    //tripal_view_file in tripal.user.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalUserFileRenew($uid, $file_id) {
    //tripal_renew_file in tripal.user.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalUserFileDownload($uid, $file_id) {
    //tripal_download_file in tripal.user.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalDashboard() {
    $output = '';
    $block_manager = \Drupal::service('plugin.manager.block');
    // You can hard code configuration or you load from settings.
    $config = [];
    $note_block = $block_manager->createInstance('notifications', $config);
    $access_result = $note_block->access(\Drupal::currentUser());
    if (is_object($access_result) && $access_result->isForbidden() || is_bool($access_result) && !$access_result) {
      return [];
    }

    $bar_chart_block = $block_manager->createInstance('content_type_bar_chart', $config);
    $access_result = $bar_chart_block->access(\Drupal::currentUser());
    if (is_object($access_result) && $access_result->isForbidden() || is_bool($access_result) && !$access_result) {
      return [];
    }

    $output .= \Drupal::service('renderer')->render($note_block->build());
    $output .= \Drupal::service('renderer')->render($bar_chart_block->build());

    return [
      '#markup' => $output,
    ];
  }

}
