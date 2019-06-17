<?php

namespace Drupal\tripal\Controller;

use Drupal\Core\Controller\ControllerBase;

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

  public function tripalAddPage() {
    // TripalEntityUIController::tripal_add_page
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
  public function tripalCheckForFields($bundle_name) {
    //@TODO This was in the module file probably needs to go elsewhere
    $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));
    $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));

    $added = tripal_create_bundle_fields($bundle, $term);
    if (count($added) == 0) {
      drupal_set_message('No new fields were added');
    }
    foreach ($added as $field_name) {
      drupal_set_message('Added field: ' . $field_name);
    }
    drupal_goto("admin/structure/bio_data/manage/$bundle_name/fields");
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
  public function tripalAdminBioDataEdit($id) {
  } 
  public function tripalAdminBioDataDelete($id) {
  } 
  public function tripalAdminBioDataAdd() {
  } 
  public function tripalAdminBioDataAddSpecific($id) {
  }      
  public function tripalAdminCVLookup($cv, $term) {
  }        
}