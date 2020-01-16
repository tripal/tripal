<?php

namespace Drupal\tripal\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Url;

/**
 * Controller routines for the Tripal Module
 */
class TripalController extends ControllerBase {
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
    //tripal_jobs_admin_view in tripal.jobs.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }
  public function tripalJobsHelp() {
    //tripal_job_help in tripal.jobs.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
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
    //tripal_jobs_view in tripal.jobs.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
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

  public function tripalFileUpload() {
    //tripal_file_upload in tripal.upload.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
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
  public function tripalFilesUsage() {
    //tripal_user_collections_view_page in tripal.collections.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
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
}
