<?php
/**
 * @file
 * Contains functions related to administrative blocks for daemon monitoring.
 */

/**
 * Implements hook_block_info().
 */
function tripal_daemon_block_info() {
  $blocks = [];

  // Status Blocks.
  $blocks['trpdaemon_status'] = [
    'info' => t('Tripal Daemon Status'),
    'cache' => DRUPAL_NO_CACHE,
  ];
  $blocks['trpdaemon_status_admin'] = [
    'info' => t('Tripal Daemon Status: ADMIN'),
    'cache' => DRUPAL_NO_CACHE,
    'status' => TRUE,
    'region' => 'dashboard_sidebar',
  ];

  // Display Log Block.
  $blocks['trpdaemon_log'] = [
    'info' => t('Tripal Daemon Log'),
    'status' => TRUE,
    'region' => 'dashboard_main',
  ];

  return $blocks;
}

/**
 * Implements hook_block_view().
 */
function tripal_daemon_block_view($delta = '') {
  $block = [];

  switch ($delta) {
    case 'trpdaemon_status_admin':
      $block['subject'] = t('Job Daemon Status');
      $block['content'] = theme_tripal_daemon_status_block_content(TRUE);
      break;
    case 'trpdaemon_status':
      $block['subject'] = t('Job Daemon Status');
      $block['content'] = theme_tripal_daemon_status_block_content();
      break;
    case 'trpdaemon_log':
      $block['subject'] = t('Job Daemon Log');
      $block['content'] = drupal_get_form('trpdaemon_display_log_form');
      break;
  }

  return $block;
}

/**
 * Provide markup for the Tripal Job Daemon Status block.
 *
 * @param $show_all
 *   A boolean indicating whether to show administrative detail (TRUE) or not
 *   (FALSE).
 *
 * @return
 *   HTML to be rendered for the block.
 */
function theme_tripal_daemon_status_block_content($show_all = FALSE) {
  $output = '';

  // Get information.
  $is_running = drushd_is_daemon_running('tripal_daemon');
  $status_file = drushd_get_daemon_status_file('tripal_daemon');
  $status = unserialize(file_get_contents($status_file));
  $PID = $status['PID'];
  $is_alive = `ps h --pid $PID | wc -l`;
  $is_alive = trim($is_alive);

  $status_class = ($is_running) ? 'active' : 'inactive';
  $status_class = ($is_running AND !$is_alive) ? 'dead' : $status_class;

  // Theme content.
  drupal_add_css(drupal_get_path('module', 'tripal_daemon') . '/theme/status_block.css');

  // Display the status.
  $output .= '<div class="daemon-status">';
  if ($is_running and $is_alive) {
    $output .= theme_image([
      'path' => 'misc/message-24-ok.png',
      'alt' => 'status-ok',
      'attributes' => [],
    ]);
    if ($status['Running Job']) {
      $output .= 'Running Job(s)';
    }
    else {
      $output .= 'Waiting for Job';
    }
  }
  else {
    $output .= theme_image([
      'path' => 'misc/message-24-error.png',
      'alt' => 'status-error',
      'attributes' => [],
    ]);
    if ($is_running AND !$is_alive) {
      $output .= 'Dead';
    }
    else {
      $output .= 'Stopped';
    }
  }
  $output .= '</div>';

  // If asked, show all the details.
  if ($show_all) {
    $output .= '<ul>';
    foreach ($status as $k => $v) {

      // If it's a boolean, then make it readable.
      if (is_bool($v)) {
        $v = ($v) ? 'True' : 'False';
      }

      // If these are current jobs then we want to link to details.
      if ($k == 'Current Jobs' AND !empty($v)) {
        $list = [];
        foreach ($v as $job_id) {
          $url = 'admin/tripal/tripal_jobs/view/' . $job_id;
          $list[$job_id] = l($job_id, $url);
        }
        $v = $list;
      }

      // If it's an array then make it a list.
      if (is_array($v)) {
        if (empty($v)) {
          $v = 'None';
        }
        else {
          $v = implode(', ', $v);
        }
      }

      $output .= '<li><strong>' . $k . '</strong>: ' . $v . '</li>';
    }
    $output .= '</ul>';
  }

  return '<div class="inner ' . $status_class . '">' . $output . '</div>';
}

/**
 * Form to display a user selected number of lines from the Tripal Job Daemon
 * log file.
 */
function trpdaemon_display_log_form($form, $form_state) {

  $form['#attached']['css'][] = drupal_get_path('module', 'tripal_daemon') . '/theme/tripal_daemon.log_block.css';

  $status_file = drushd_get_daemon_status_file('tripal_daemon');
  if ($status_file) {
    $status = unserialize(file_get_contents($status_file));
    $file = $status['Current Log File'];
  }
  else {
    $file = NULL;
  }

  $form['num_lines'] = [
    '#type' => 'radios',
    '#title' => 'Lines',
    '#description' => 'The number of lines to display from the end of the Tripal Job Daemon Log file.',
    '#options' => [
      '10' => '10',
      '25' => '25',
      '50' => '50',
      '100' => '100',
      '200' => '200',
      '500' => '500',
    ],
    '#default_value' => '25',
    '#attributes' => [
      'onChange' => 'this.form.submit();',
      'class' => ['container-inline'],
    ],
  ];

  if ($file) {
    $num_lines = (isset($form_state['values'])) ? $form_state['values']['num_lines'] : $form['num_lines']['#default_value'];
    $text = `tail -n $num_lines $file`;
    $text = str_replace("\n", '<br />', $text);
  }
  else {
    $num_lines = 0;
    $text = '';
  }

  $form['log'] = [
    '#type' => 'markup',
    '#markup' => $text,
    '#prefix' => '<pre id="daemon-log">',
    '#suffix' => '</pre>',
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => t('Apply'),
    '#attributes' => [
      'style' => ['display: none;'],
    ],
  ];

  return $form;
}

/**
 * Display Log Form: Submit.
 */
function trpdaemon_display_log_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}
