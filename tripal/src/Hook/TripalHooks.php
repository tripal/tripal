<?php

namespace Drupal\tripal\Hook;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\Core\Url;
use Drupal\tripal\TripalField\TripalFieldItemBase;

/**
 * Hook implementations for the Tripal module.
 *
 * This class contains miscellaneous hook implementations that do not fit in
 * any of the other hook classes in this namespace. Only include hooks here
 * if they do not fit better elsewhere.
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
   * Implements hook_rebuild().
   */
  #[Hook('rebuild')]
  public function rebuild(): string {
    \Drupal::service('tripal.rebuild_service')->executeRebuild();
    // Return value of the module name is only used for phpunit tests.
    return 'tripal';
  }

  /**
   * Implements hook_toolbar().
   *
   * This hook is provided by the Gin Admin theme and we use it to provide
   * the specialized Tripal toolbar in their specialized top menu bar.
   */
  #[Hook('toolbar')]
  public function toolbar() {
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
   * Implements hook_form_alter for the field_config_edit_form.
   *
   * Adds the form elements to the field settings form for setting controlled
   * vocabulary. The function only does this for fields that are not
   * Tripal fields but which are attached to a tripal content type.
   */
  #[Hook('form_alter')]
  public function formFieldConfigEditFormAlter(&$form, FormStateInterface $form_state, $form_id) {
    if ($form_id === 'field_config_edit_form') {
      $form_object = $form_state->getFormObject();
      if (method_exists($form_object, 'getEntity')) {
        /** @var \Drupal\field\Entity\FieldConfig $field **/
        $field = $form_object->getEntity();
        if (method_exists($field, 'getTargetEntityTypeId')) {
          if ($field->getTargetEntityTypeId() == 'tripal_entity') {
            $elements = TripalFieldItemBase::buildFieldTermForm($field, $form, $form_state);
            $form['settings'] = $form['settings'] + $elements;
          }
        }
      }
    }
  }

  /**
   * Implements hook gin_content_form_routes provided by the GIN admin theme.
   */
  #[Hook('gin_content_form_routes')]
  public function ginContentFormRoutes() {
    return [
      'entity.tripal_entity.add_form',
      'entity.tripal_entity.edit_form',
    ];
  }

}
