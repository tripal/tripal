<?php

namespace Drupal\tripal\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Hook implementations for the Tripal module.
 */
class TripalHooks {

  use StringTranslationTrait;

  /**
   * Implements hook_help().
   */
  #[Hook('help')]
  public function help($route_name, RouteMatchInterface $route_match): string|array|null {
    switch ($route_name) {
      // Main module help for the tripal module.
      case 'help.page.tripal':
        $output = '<h3>' . $this->t('About') . '</h3>';
        $output .= '<p>' . $this->t('Tripal is a toolkit to facilitate construction of online genomic, genetic (and other biological) websites.') . '</p>';
        return ['#markup' => $output];
    }
    return NULL;
  }

  /**
   * Implements hook_page_attachments().
   */
  #[Hook('page_attachments')]
  public function addPageAttachments(array &$attachments): void {
    $attachments['#attached']['drupalSettings']['tripal']['vars'] = [
      'baseurl' => \Drupal::request()->getSchemeAndHttpHost(),
      'tripal_path' => \Drupal::service('extension.list.module')->getPath('tripal'),
    ];
    $attachments['#attached']['library'][] = 'tripal/vars';
  }

  /**
   * Implements hook_rebuild().
   */
  #[Hook('rebuild')]
  public function onRebuild(): void {
    \Drupal::service('tripal.rebuild_service')->executeRebuild();
  }

}
