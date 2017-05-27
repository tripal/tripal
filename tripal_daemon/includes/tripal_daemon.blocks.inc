<?php
/** 
 * @file
 * Contains functions related to administrative blocks for daemon monitoring.
 */

/**
 * Implements hook_block_info().
 */
function tripal_daemon_block_info() {
  $blocks = array();

  $blocks['trpdaemon_status'] = array(
    'info' => t('Tripal Daemon Status'),
  );

  $blocks['trpdaemon_status_admin'] = array(
    'info' => t('Tripal Daemon Status: ADMIN'),
  );
  
  return $blocks;
}

/**
 * Implements hook_block_view().
 */
function tripal_daemon_block_view($delta='') {
  $block = array();

  switch($delta) {
    case 'trpdaemon_status_admin':
      $block['subject'] = t('Job Daemon Status');
      $block['content'] = theme_tripal_daemon_status_block_content(TRUE);
      break;
    case 'trpdaemon_status':
      $block['subject'] = t('Job Daemon Status');
      $block['content'] = theme_tripal_daemon_status_block_content();
      break;
  }
  
  return $block;
}

/** 
 * Provide markup for the Tripal Job Daemon Status block.
 *
 * @param $show_all
 *   A boolean indicating whether to show administrative detail (TRUE) or not (FALSE).
 * @return
 *   HTML to be rendered for the block.
 */
function theme_tripal_daemon_status_block_content($show_all = FALSE) {
  $output = '';

  // Get information.
  $is_running = drushd_is_daemon_running('tripal_daemon');
  $status_file = drushd_get_daemon_status_file('tripal_daemon');
  $status = unserialize(file_get_contents($status_file));

  $status_class = ($is_running) ? 'active' : 'inactive';

  // Theme content.
  drupal_add_css(drupal_get_path('module','tripal_daemon') . '/theme/status_block.css');

  // Display the status.
  $output .= '<div class="daemon-status">';
  if ($is_running) {
    $output .= theme_image(array(
      'path' => 'misc/message-24-ok.png',
      'alt' => 'status-ok',
    ));
    $output .= 'Running';
  }
  else {
    $output .= theme_image(array(
      'path' => 'misc/message-24-error.png',
      'alt' => 'status-error',
    ));
    $output .= 'Stopped';
  }
  $output .= '</div>';

  // If asked, show all the details.
  if ($show_all) {
    $output .= '<ul>';
    foreach ($status as $k => $v) {
      if (is_bool($v)) {
        $v = ($v) ? 'True' : 'False';
      }

      $output .= '<li><strong>' . $k . '</strong>: ' . $v . '</li>';
    }
    $output .= '</ul>';
  }

  return '<div class="inner '.$status_class.'">' . $output . '</div>';
}
