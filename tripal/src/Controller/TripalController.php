<?php

namespace Drupal\tripal\Controller;

use Drupal\Core\Breadcrumb\Breadcrumb;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Core\Render\Markup;
use Drupal\file\Entity\File;
use Drupal\user\Entity\User;
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

  public function tripalUserFiles(User $user) {
    //tripal_user_files_page in tripal.user.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalUserFileDetails(User $user, File $file) {
    //tripal_view_file in tripal.user.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalUserFileRenew(User $user, File $file) {
    //tripal_renew_file in tripal.user.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalUserFileDownload(User $user, File $file) {
    //tripal_download_file in tripal.user.inc
    return [
      '#markup' => 'Not yet upgraded.',
    ];
  }

  public function tripalUserFileDelete(User $user, File $file) {
    //tripal_delete_file in tripal.user.inc
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

    $built_note_block = $note_block->build();
    $output .= \Drupal::service('renderer')->render($built_note_block);

    $built_bar_chart_block = $bar_chart_block->build();
    $output .= \Drupal::service('renderer')->render($built_bar_chart_block);

    return [
      '#markup' => $output,
    ];
  }

}
