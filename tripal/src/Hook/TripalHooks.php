<?php

namespace Drupal\tripal\Hook;

use Symfony\Component\HttpKernel\Exception\AccessDeniedHttpException;
use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Render\Markup;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\Core\Url;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\views\ViewEntityInterface;
use Drupal\views\ViewExecutable;
use Drupal\views\ViewsConfigUpdater;

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
   * Implements hook_form_alter for the field_config_edit_form.
   *
   * Adds the form elements to the field settings form for setting controlled
   * vocabulary. The function only does this for fields that are not
   * Tripal fields but which are attached to a tripal content type.
   */
  #[Hook('form_alter')]
  public function formFieldConfigEditFormAlter(&$form, FormStateInterface $form_state, $form_id) {
    /** @var \Drupal\field\Entity\FieldConfig $field **/
    $field = $form_state->getFormObject()->getEntity();
    if ($field->getTargetEntityTypeId() == 'tripal_entity') {
      $elements = TripalFieldItemBase::buildFieldTermForm($field, $form, $form_state);
      $form['settings'] = $form['settings'] + $elements;
    }
  }

  /**
   * Implements hook_preprocess_field().
   *
   * This is to permit HTML markup in the page title field, for
   * example on an organism page, to display genus and species
   * in italics.
   */
  #[Hook('preprocess_field')]
  public function preprocessField(&$variables) {
    if ($variables['element']['#field_name'] == 'title') {
      // We can configure which tags are allowed at /admin/tripal/config.
      $tag_string = \Drupal::config('tripal.settings')->get('tripal_entity_type.allowed_title_tags');
      $tripal_allowed_tags = explode(' ', $tag_string ?? '');

      // Process each item (usually only a single one).
      foreach ($variables['items'] as $delta => $item) {
        // The title can be either a simple inline template or a link.
        if ($item['content']['#type'] == 'inline_template') {
          $value = $item['content']['#context']['value'];
          // Convert strings into markup, this will cause HTML tags to
          // be rendered.
          if (is_string($value)) {
            $sanitized_value = Xss::filter($value, $tripal_allowed_tags);
            $variables['items'][$delta]['content']['#context']['value'] = Markup::create($sanitized_value);
          }
        }
        elseif ($item['content']['#type'] == 'link') {
          $value = $item['content']['#title']['#context']['value'];
          // Convert strings into markup, this will cause HTML tags to
          // be rendered.
          if (is_string($value)) {
            $sanitized_value = Xss::filter($value, $tripal_allowed_tags);
            $variables['items'][$delta]['content']['#title']['#context']['value'] = Markup::create($sanitized_value);
          }
        }
      }
    }
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

    $theme['tripal_entity'] = [
      'render element' => 'elements',
      'file' => 'templates/tripal_entity.page.php',
      'template' => 'tripal_entity',
    ];

    $theme['tripal_entity_edit_form'] = [
      'render element' => 'form',
      'template' => 'tripal-entity-edit-form',
    ];

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
  public function themeSuggestionsTripalEntity(array $variables) {
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

  /**
   * Implements hook_views_pre_view().
   */
  #[Hook('views_pre_view')]
  public function viewsPreView(ViewExecutable $view, $display_id, array &$args) {
    // Override permission of Tripal Content Type listing
    // (admin/content/bio_data) in order to support OR when evaluating
    // permissions since views UI does not support this.
    // WARNING: requires you to have NO Permissions on this view through the UI!
    if ($view->id() == 'tripal_content_type_listing') {
      // The user must have 1+ of the following permissions to access the
      // Tripal Content Type listing (admin/content/bio_data).
      $perm = [
        'access tripal content overview',
        'administer tripal content',
      ];

      // Check if current user has either permissions when attempting to
      // view listing.
      $user = \Drupal::currentUser();

      $has_permission = FALSE;
      foreach ($perm as $permission) {
        if ($user->hasPermission($permission)) {
          $has_permission = TRUE;
          break;
        }
      }

      // Deny access if user does not have necessary permission.
      if (!$has_permission) {
        throw new AccessDeniedHttpException();
      }
    }
  }

  /**
   * Implements hook_ENTITY_TYPE_presave().
   *
   * A new views 'class' setting was introduced in Drupal 11.2, but it has
   * not been added to our yaml views configurations so that we maintain
   * backward compatibility. We do the update here before Drupal does it
   * in order to avoid a deprecation notice in unit tests.
   * This hook will do nothing for Drupal <11.2.
   */
  #[Hook('view_presave')]
  public function viewPresave(ViewEntityInterface $view): void {
    /** @var \Drupal\views\ViewsConfigUpdater $config_updater */
    $config_updater = \Drupal::classResolver(ViewsConfigUpdater::class);
    if (method_exists($config_updater, 'processTableCssClassUpdate')) {
      $config_updater->setDeprecationsEnabled(FALSE);
      $config_updater->processTableCssClassUpdate($view);
    }
  }

}
