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
  }
  public function tripalExtensions() {
    //
  }
  public function tripalJobs() {
    //tripal_jobs_admin_view in tripal.jobs.inc
  }
  public function tripalJobsHelp() {
    //tripal_job_help in tripal.jobs.inc
  }
  public function tripalJobsCancel($id) {
    //tripal_cancel_job in tripal.jobs.api.inc
  }
  public function tripalJobsStatus($id) {
    //tripal_jobs_status_view in tripal.jobs.inc
  }
  public function tripalJobsRerun($id) {
    //tripal_rerun_job in tripal.jobs.inc
  }
  public function tripalJobsView($id) {
    //tripal_jobs_view in tripal.jobs.inc
  }
  public function tripalJobsEnable() {
    // tripal_enable_view in tripal.jobs.inc
  }
  public function tripalJobsExecute($id) {
    //tripal_jobs_view in tripal.jobs.inc
  }
  public function tripalAttachField($id) {
    //tripal_jobs_view in tripal.jobs.inc
  }
  public function tripalDisableNotification($id) {
    //tripal_disable_admin_notification in tripal.admin_blocks.inc
  }
  public function tripalFieldNotification($field_name_note, $bundle_id, $module, $field_or_instance) {
    //tripal_admin_notification_import_field in tripal.admin_blocks.inc
  }
  public function tripalCvLookup() {
    //tripal_vocabulary_lookup_vocab_page in tripal.term_lookup.inc
  }
  public function tripalCVTerm($vocabulary, $accession) {
    //tripal_vocabulary_lookup_term_page in tripal.term_lookup.inc
  }
  public function tripalCVTermChildren($vocabulary, $accession) {
    //tripal_vocabulary_lookup_term_children_ajax in tripal.term_lookup.inc
  }

  public function tripalFileUpload() {
    //tripal_file_upload in tripal.upload.inc
  }
  public function tripalDataLoaders() {
    //tripal_file_upload in tripal.upload.inc
  }

  //@TODO A controller to process the TripalRouters for dataLoaders.

  public function tripalDataCollections() {
    //tripal_user_collections_view_page in tripal.collections.inc
  }
  public function tripalFilesUsage() {
    //tripal_user_collections_view_page in tripal.collections.inc
  }
  public function tripalDataCollectionsView() {
    //tripal_admin_file_usage_page in tripal.admin_files.inc
  }
  public function tripalUserFiles($uid) {
    //tripal_user_files_page in tripal.user.inc
  }
  public function tripalUserFileDetails($uid, $file_id) {
    //tripal_view_file in tripal.user.inc
  }
  public function tripalUserFileRenew($uid, $file_id) {
    //tripal_renew_file in tripal.user.inc
  }
  public function tripalUserFileDownload($uid, $file_id) {
    //tripal_download_file in tripal.user.inc
  }
  public function tripalAdminCVLookup($cv, $term) {
  }
}
