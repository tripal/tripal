<?php

namespace Drupal\tripal\Hook;

use Drupal\Core\Hook\Attribute\Hook;
use Drupal\Core\Render\Element;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Theme hook implementations for the Tripal module.
 *
 * This class contains hook implementations pertaining to theme and render
 * functionality. Some examples include hooks:
 *  - with theme or render in the name.
 *  - preprocessing variables for twig templates.
 *  - adding js/css libraries or settings.
 */
class TripalThemeHooks {

  use StringTranslationTrait;

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
   * Implements hook_theme().
   */
  #[Hook('theme')]
  public function theme() {
    $theme = [];

    $theme['tripal_entity_type'] = [
      'render element' => 'elements',
      'initial preprocess' => static::class . ':templatePreprocessTripalEntityType',
      'template' => 'tripal_entity_type',
    ];

    $theme['tripal_entity'] = [
      'render element' => 'elements',
      'initial preprocess' => static::class . ':templatePreprocessTripalEntity',
      'template' => 'tripal_entity',
    ];

    $theme['tripal_entity_edit_form'] = [
      'render element' => 'form',
      'template' => 'tripal-entity-edit-form',
    ];

    $theme['tripal_entity_content_add_list'] = [
      'render element' => 'types',
      'variables' => ['types' => NULL],
      'initial preprocess' => static::class . ':templatePreprocessTripalEntity',
    ];

    return $theme;
  }

  /**
   * Prepares variables for Tripal Entity Type templates.
   *
   * Default template: tripal_entity_type.html.twig.
   *
   * @param array &$variables
   *   An associative array containing:
   *   - elements: An associative array containing the user information.
   *   - attributes: HTML attributes for the containing element.
   */
  public function templatePreprocessTripalEntityType(array &$variables): void {

    // Fetch TripalEntityType Object.
    $tripal_entity_type = $variables['elements']['#tripal_entity_type'];

    // Take information from the entity and add it to the content.
    $variables['content']['label'] = [
      '#type' => 'item',
      '#title' => 'Label',
      '#markup' => $tripal_entity_type->getLabel(),
      '#wrapper_attributes' => [
        'class' => ['container-inline'],
      ],
    ];
    $variables['content']['term'] = [
      '#type' => 'item',
      '#title' => 'Term',
      '#markup' => $tripal_entity_type->getTermIdSpace() . ':' . $tripal_entity_type->getTermAccession(),
      '#wrapper_attributes' => [
        'class' => ['container-inline'],
      ],
    ];
    $variables['content']['category'] = [
      '#type' => 'item',
      '#title' => 'Category',
      '#markup' => $tripal_entity_type->getCategory(),
      '#wrapper_attributes' => [
        'class' => ['container-inline'],
      ],
    ];
    $variables['content']['description'] = [
      '#type' => 'item',
      '#title' => 'Help Text for Curators',
      '#markup' => $tripal_entity_type->getHelpText(),
    ];

    // Helpful $content variable for templates.
    // Only adds fields which TripalEntityType may not have.
    foreach (Element::children($variables['elements']) as $key) {
      $variables['content'][$key] = $variables['elements'][$key];
    }
  }

  /**
   * Prepares variables for Tripal Content templates.
   *
   * Default template: tripal_entity.html.twig.
   *
   * @param array $variables
   *   An associative array containing:
   *   - elements: An associative array containing the user information and any
   *   - attributes: HTML attributes for the containing element.
   */
  public function templatePreprocessTripalEntity(array &$variables) {

    // Helpful $content variable for templates.
    if (array_key_exists('elements', $variables)) {
      foreach (Element::children($variables['elements']) as $key) {
        $variables['content'][$key] = $variables['elements'][$key];
      }
    }
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
   * Implements hook_preprocess_html().
   *
   * Replacement for tripal_init.
   *
   * @param array &$variables
   *   An associative array containing:
   *   - page: A render element representing the page.
   */
  #[Hook('preprocess_html')]
  public function preprocessHtml(&$variables) {
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
    // @see Issue #2360.
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

}
