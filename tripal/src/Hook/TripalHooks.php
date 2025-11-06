<?php

namespace Drupal\tripal\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\Core\Url;

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

  /**
   * Implements hook_toolbar().
   */
  #[Hook('toolbar')]
  public function tripalToolbar() {
    $items = [];

    $items['tripal'] = [
      '#cache' => [
        'contexts' => [
          'user.permissions',
        ],
      ],
    ];

    if (!\Drupal::currentUser()->hasPermission('administer tripal')) {
      return $items;
    }

    $items['tripal'] += [
      '#type' => 'toolbar_item',
      'tab' => [
        '#type' => 'link',
        '#title' => $this->t('Tripal'),
        '#url' => Url::fromRoute('tripal.admin'),
        '#attributes' => [
          'title' => $this->t('Tripal administration'),
          'class' => ['toolbar-icon', 'toolbar-icon-tripal'],
          'aria-pressed' => 'false',
        ],
      ],
      'tray' => [
        '#heading' => $this->t('Tripal Administration'),
        'tripal-toolbar-tray' => [
          '#prefix' => '<ul class="toolbar-menu">',
          '#suffix' => '</ul>',
          'content' => [
            '#type' => 'link',
            '#title' => $this->t('Content'),
            '#url' => Url::fromRoute('entity.tripal_entity.collection'),
            '#attributes' => [
              'title' => 'Find biological content pages.',
              'class' => ['toolbar-icon', 'toolbar-icon-tripal-content'],
            ],
            '#prefix' => '<li class="menu-item">',
            '#suffix' => '</li>',
          ],
          'jobs' => [
            '#type' => 'link',
            '#title' => $this->t('Jobs'),
            '#url' => Url::fromRoute('tripal.jobs'),
            '#attributes' => [
              'title' => 'Configure the form/page display for biological content.',
              'class' => ['toolbar-icon', 'toolbar-icon-tripal-jobs'],
            ],
            '#prefix' => '<li class="menu-item">',
            '#suffix' => '</li>',
          ],
          'importers' => [
            '#type' => 'link',
            '#title' => $this->t('Data Loaders'),
            '#url' => Url::fromRoute('tripal.data_loaders'),
            '#attributes' => [
              'title' => 'Importers for biological content.',
              'class' => ['toolbar-icon', 'toolbar-icon-tripal-importers'],
            ],
            '#prefix' => '<li class="menu-item">',
            '#suffix' => '</li>',
          ],
          'page_structure' => [
            '#type' => 'link',
            '#title' => $this->t('Page Structure'),
            '#url' => Url::fromRoute('entity.tripal_entity_type.collection'),
            '#attributes' => [
              'title' => 'Configure the form/page display for biological content.',
              'class' => ['toolbar-icon', 'toolbar-icon-tripal-structure'],
            ],
            '#prefix' => '<li class="menu-item">',
            '#suffix' => '</li>',
          ],
          /*
          'vocabs' => [
            '#type' => 'link',
            '#title' => $this->t('Vocabularies'),
            '#url' => Url::fromRoute('entity.tripal_vocab.collection'),
            '#attributes' => [
              'title' => 'The ontological structure behind your Tripal site.',
              'class' => ['toolbar-icon','toolbar-icon-tripal-vocab'],
            ],
            '#prefix' => '<li class="menu-item">',
            '#suffix' => '</li>',
          ],
           */
          'storage' => [
            '#type' => 'link',
            '#title' => $this->t('Data Storage'),
            '#url' => Url::fromRoute('tripal.storage'),
            '#attributes' => [
              'title' => 'Flexible data storage for your biological content.',
              'class' => ['toolbar-icon', 'toolbar-icon-tripal-storage'],
            ],
            '#prefix' => '<li class="menu-item">',
            '#suffix' => '</li>',
          ],
          'extensions' => [
            '#type' => 'link',
            '#title' => $this->t('Extensions'),
            '#url' => Url::fromRoute('tripal.extension'),
            '#attributes' => [
              'title' => 'Provides administration of Tripal Extension modules.',
              'class' => ['toolbar-icon', 'toolbar-icon-tripal-extend'],
            ],
            '#prefix' => '<li class="menu-item">',
            '#suffix' => '</li>',
          ],
          'more' => [
            '#type' => 'link',
            '#title' => $this->t('more'),
            '#url' => Url::fromRoute('tripal.admin'),
            '#attributes' => [
              'title' => 'For additional Tripal administration.',
              'class' => ['toolbar-icon', 'toolbar-icon-tripal-more'],
            ],
            '#prefix' => '<li class="menu-item">',
            '#suffix' => '</li>',
          ],
        ],
      ],
      '#attached' => [
        'library' => 'tripal/tripal-toolbar',
      ],
    ];

    // Determine which tray item should be active
    // based on the current page.
    Url::fromRoute('<current>', [], ['absolute' => 'true'])->toString();
    $curr_route_name = \Drupal::routeMatch()->getRouteName();

    // Tray items routeNames (#url key in the toolbar tray definition above).
    $tray_items = [
      'entity.tripal_entity.collection' => 'content',
      'tripal.jobs' => 'jobs',
      'tripal.data_loaders' => 'importers',
      'entity.tripal_entity_type.collection' => 'page_structure',
      // 'entity.tripal_vocab.collection' => 'vocab',
      'tripal.storage' => 'storage',
      'tripal.extension' => 'extensions',
      'tripal.admin' => 'more',
    ];

    foreach ($tray_items as $route_name => $page_name) {
      if ($route_name == $curr_route_name) {
        array_push($items['tripal']['tray']['tripal-toolbar-tray'][$page_name]['#attributes']['class'], 'is-active');
        break;
      }
    }

    return $items;
  }

  /**
   * Implements hook_preprocess_html().
   *
   * Replacement for tripal_init.
   *
   * @param array &$variables
   *   An associative array containing:
   *   - page: A render element representing the page.
   */
  #[Hook('preprocess_html')]
  public function tripalPreprocessHtml(&$variables) {
    global $base_url;
    // @todo Need to look into service injection in the module file.
    $config = \Drupal::config('tripal_admin.settings');
    // Add some variables for all javasript to use for building URLs.
    $clean_urls = $config->get('clean_url', 0);
    $tripal_path = \Drupal::service('extension.list.module')->getPath('tripal');
    // Add a JS library.
    $variables['#attached']['library'][] = 'tripal/tripal-js';
    $variables['#attached']['drupalSettings']['tripal']['tripalJS']['baseurl'] = $base_url;
    $variables['#attached']['drupalSettings']['tripal']['tripalJS']['isClean'] = $clean_urls;
    $variables['#attached']['drupalSettings']['tripal']['tripalJS']['tripal_path'] = $tripal_path;
    // Make sure the date time settings are the way Tripal will insert them,
    // otherwise PostgreSQL version that may have a different datestyle setting
    // will fail when inserting or updating a date column in a table.
    // @todo we do this for every page? Maybe move to rebuild().
    \Drupal::database()->query("SET DATESTYLE TO :style", [':style' => 'MDY']);

    // Ask users to do the registration form.
    // @upgrade when Issue #45 is closed.
    // if (\Drupal::currentUser()->hasPermission('administer tripal')) {
    // if (empty($config->get('tripal_site_registration') ?: FALSE)
    // || !($config->get('disable_tripal_reporting') ?: FALSE)) {
    // \Drupal::messenger()->addWarning(t('Please register your Tripal Site.
    // Registering provides important information that will help secure funding
    // for continued improvements to Tripal.
    // <a href="admin/tripal/register">Click to register now or opt out</a>.'));
    // }
    // }
  }

  /**
   * Implements hook_theme().
   */
  #[Hook('theme')]
  public function tripalTheme() {
    $theme = [];

    $theme['tripal_entity_type'] = [
      'render element' => 'elements',
      'file' => 'templates/tripal_entity_type.page.php',
      'template' => 'tripal_entity_type',
    ];

    $theme['tripal_entity'] = array(
      'render element' => 'elements',
      'file' => 'templates/tripal_entity.page.php',
      'template' => 'tripal_entity',
    );

    $theme['tripal_entity_edit_form'] = array(
      'render element' => 'form',
      'template' => 'tripal-entity-edit-form',
    );

    $theme['tripal_entity_content_add_list'] = [
      'render element' => 'types',
      'variables' => ['types' => NULL],
      'file' => 'templates/tripal_entity.page.php',
    ];

    return $theme;
  }

  /**
   * Implements hook_theme_suggestions_HOOK().
   */
  #[Hook('theme_suggestions_tripal_entity')]
  function themeSuggestionsTripalEntity(array $variables) {
    $suggestions = [];
    $entity = $variables['elements']['#tripal_entity'];
    $sanitized_view_mode = strtr($variables['elements']['#view_mode'], '.', '_');
    $suggestions[] = 'tripal_entity__' . $sanitized_view_mode;
    $suggestions[] = 'tripal_entity__' . $entity->bundle();
    $suggestions[] = 'tripal_entity__' . $entity->bundle() . '__' . $sanitized_view_mode;
    $suggestions[] = 'tripal_entity__' . $entity->id();
    $suggestions[] = 'tripal_entity__' . $entity->id() . '__' . $sanitized_view_mode;
    return $suggestions;
  }

}
