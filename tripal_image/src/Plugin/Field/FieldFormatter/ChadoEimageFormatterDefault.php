<?php

namespace Drupal\tripal_image\Plugin\Field\FieldFormatter;

use Drupal\Component\Render\FormattableMarkup;
use Drupal\Core\DependencyInjection\ContainerFactoryPluginInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Pager\PagerManagerInterface;
use Drupal\Core\Render\Markup;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\Url;
use Drupal\image\Entity\ImageStyle;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * The default formatter for chado image content type.
 */
#[TripalFieldFormatter(
  id: 'chado_eimage_formatter_default',
  label: new TranslatableMarkup('Chado Image Formatter'),
  description: new TranslatableMarkup('Displays images stored in the chado eimage table'),
  field_types: [
    'chado_eimage_type_default',
  ],
  valid_tokens: [
    '[name]',
    '[value]',
    '[rank]',
  ],
)]
class ChadoEimageFormatterDefault extends ChadoFormatterBase {

  protected $pagerManager;

  public function __construct($plugin_id, $plugin_definition, $field_definition, $settings, $label, $view_mode, $third_party_settings, PagerManagerInterface $pager_manager) {
    parent::__construct($plugin_id, $plugin_definition, $field_definition, $settings, $label, $view_mode, $third_party_settings);
    $this->pagerManager = $pager_manager;
  }

  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $plugin_id,
      $plugin_definition,
      $configuration['field_definition'],
      $configuration['settings'],
      $configuration['label'],
      $configuration['view_mode'],
      $configuration['third_party_settings'],
      $container->get('pager.manager')
    );
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['token_string'] = '<strong>[name]</strong>: [value]';
    $settings['tripal_image_disable_link_to_entity'] = FALSE;
    $settings['tripal_image_max_thumbnail_height'] = '200px';
    $settings['tripal_image_thumbnail_regex_pattern'] = '';
    $settings['tripal_image_thumbnail_regex_replacement'] = '';
    $settings['tripal_image_items_per_page'] = 10;
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    $list = [];
    $token_string = $this->getSetting('token_string');
    $disable_link = $this->getSetting('tripal_image_disable_link_to_entity');
    $max_thumbnail_height = $this->getSetting('tripal_image_max_thumbnail_height') ?: '200px';
    $items_per_page = $this->getSetting('tripal_image_items_per_page');
    $regex_pattern = $this->getSetting('tripal_image_thumbnail_regex_pattern');
    $regex_replacement = $this->getSetting('tripal_image_thumbnail_regex_replacement');
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');
    $url_generator = \Drupal::service('file_url_generator');

    // Attach css for this field.
    $elements['#attached']['library'][] = 'tripal_image/image_formatter';

    foreach ($items as $delta => $item) {
      $values = [];
      foreach ($item->getProperties() as $key => $property) {
        $values[$key] = $property->getString();
      }
      if ($values['eimage_properties']) {
        $values['eimage_properties'] = json_decode($values['eimage_properties'], TRUE);
      }

      // The assumption is that images are either stored directly in the
      // eimage table as uuencoded data, or are linked using the image_uri
      // column, but not both at the same time.
      // @todo we could store thumbnails in the table and full res. in uri.
      $thumbnail_markup = '';
      $href_url = '';
      if ($values['image_uri']) {
        $image_uri = $values['image_uri'];
        $thumbnail_uri = $image_uri;
        if ($regex_pattern) {
          $test_uri = preg_replace('#' . $regex_pattern . '#', $regex_replacement, $image_uri);
          $absolute_path = \Drupal::service('file_system')->realpath($test_uri);
          if (file_exists($absolute_path)) {
            $thumbnail_uri = $test_uri;
          }
        }
        $image_url = $url_generator->generateAbsoluteString($image_uri);
        $thumbnail_url = $url_generator->generateAbsoluteString($thumbnail_uri);
        $basename = basename($image_url);
        $thumbnail_markup = '<img src="' . $thumbnail_url . '" alt="' . $basename . '">';
        $href_url = $thumbnail_url;
      }
      elseif ($values['eimage_data']) {
        // Remove returns if the browser has added on edit.
        $values['eimage_data'] = str_replace("\r", '', $values['eimage_data']);

        // Convert from uuencode to base64.
        $binary_data = convert_uudecode($values['eimage_data']);
        $base64data = base64_encode($binary_data);

        $mime_type = 'image/' . $values['eimage_type'];
        $img_src = 'data:' . $mime_type . ';base64,' . $base64data;
        // To make the image clickable, this duplicates the image source,
        // so could it be improved?
        $thumbnail_markup = '<img src="' . $img_src . '" alt="Decoded UUencoded Image">';
        $href_url = $img_src;
      }
      $image_markup = NULL;
      if ($thumbnail_markup) {
        if ($values['entity_id'] > 0 && !$disable_link) {
          $href_url = Url::fromRoute('entity.tripal_entity.canonical', ['tripal_entity' => $values['entity_id']])->toString();
        }
        $image_markup = Markup::create('<a href="' . $href_url . '">' . $thumbnail_markup . '</a>');
      }

      // Properties are converted to a list.
      // We only have three possible tokens.
      $property_list = [];
      if ($values['eimage_properties']) {
        foreach ($values['eimage_properties'] as $label => $items) {
          $token_values = ['name' => $label];
          // Ranks were used for ordering but are not displayed by default.
          foreach ($items as $rank => $value) {
            $token_values['rank'] = $rank;
            $token_values['value'] = $value;
            // Substitute values in token string to generate displayed string.
            $displayed_string = $token_string;
            foreach ($token_values as $key => $value) {
              $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
            }
            $property_list[] = ['#markup' => $displayed_string];
          }
        }
      }

      // If the image has one or more properties, split side-by-side for
      // diplaying them, otherwise the image can take up the full width.
      // A link to the corresponding entity when one exists is not
      // implemented because eimage is not a content type.
      if ($property_list) {
        $list[$delta] = [
          '#type' => 'container',
          '#attributes' => [
            'class' => ['side-by-side-wrapper'],
          ],
          'left_content' => [
            '#type' => 'container',
            '#markup' => $image_markup,
            '#attributes' => [
              'class' => ['side-by-side-left'],
              'style' => ['--tripal-image-max-height: ' . $max_thumbnail_height],
            ],
          ],
          'right_content' => [
            '#type' => 'container',
            '#attributes' => [
              'class' => ['side-by-side-right'],
            ],
            'list' => [
              '#theme' => 'item_list',
              '#list_type' => 'ul',
              '#items' => $property_list,
            ],
          ],
        ];
      }
      else {
        $list[$delta] = [
          '#type' => 'container',
          '#markup' => $image_markup,
          '#attributes' => [
            'style' => ['--tripal-image-max-height: ' . $max_thumbnail_height],
          ],
        ];
      }
    }

    // If only one element has been found, don't make into a list.
    $total_items = count($list);
    if ($total_items == 1) {
      $elements = $list;
    }

    // If more than one value has been found, display all values in an
    // unordered list.
    elseif ($total_items > 1) {
      $paged_list = $list;
      if ($items_per_page > 0) {
        $pager = $this->pagerManager->createPager($total_items, $items_per_page);
        $current_page = $pager->getCurrentPage();
        // Slice the values for the current page
        $paged_list = array_slice($list, $current_page * $items_per_page, $items_per_page);
      }
      $elements[0] = [
        '#type' => 'container',
        '#attributes' => [
          'class' => ['image-list'],
        ],
        'list' => [
          '#theme' => 'item_list',
          '#list_type' => 'ul',
          '#items' => $paged_list,
          '#attributes' => [
            'class' => ['image-item'],
          ],
        ],
      ];
      if ($items_per_page > 0) {
        $elements[0]['pager']['#type'] = 'pager';
      }
    }

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);

   $form['tripal_image_disable_link_to_entity'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Disable link from image thumbnail to entity'),
      '#description' => $this->t('If this is enabled, an image thumbnail will link to the full resolution of the image and not to an entity page for that image.'),
      '#default_value' => $this->getSetting('tripal_image_disable_link_to_entity'),
      '#required' => FALSE,
    ];

   $form['tripal_image_max_thumbnail_height'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Maximum height of a thumbnail image'),
      '#description' => $this->t('Specify a maximum height to use for displaying an image in this field, for example "200px". The image is clickable to view at full resolution.'),
      '#default_value' => $this->getSetting('tripal_image_max_thumbnail_height'),
      '#required' => FALSE,
    ];

   $form['tripal_image_thumbnail_regex_pattern'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Pattern to modify an image URL to retrieve a thumbnail image'),
      '#description' => $this->t('Specify a combination of pattern and replacement to generate a thumbnail URL from an image URL. For example, substitute "image" with "thumbnail". Do not include regex delimiter characters, e.g. use "image" not "/image/".'),
      '#default_value' => $this->getSetting('tripal_image_thumbnail_regex_pattern'),
      '#required' => FALSE,
    ];

   $form['tripal_image_thumbnail_regex_replacement'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Replacement to modify an image URL to retrieve a thumbnail image'),
      '#description' => $this->t('The replacement for the matched pattern.'),
      '#default_value' => $this->getSetting('tripal_image_thumbnail_regex_replacement'),
      '#required' => FALSE,
    ];

   $form['tripal_image_items_per_page'] = [
      '#type' => 'number',
      '#title' => $this->t('The maximum number of items to display at one time'),
      '#description' => $this->t('If there are more than the specified number of items, a pager is used to display a subset. Enter a value of 0 to disable the pager and always display all items.'),
      '#default_value' => $this->getSetting('tripal_image_items_per_page'),
      '#required' => FALSE,
      '#min' => 0,
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = parent::settingsSummary();
    $summary[] = $this->t('Max Height: @max_height',
                          ['@max_height' => $this->getSetting('tripal_image_max_thumbnail_height')]);
    $summary[] = $this->t('Thumbnail regex: @set',
                          ['@set' => $this->getSetting('tripal_image_thumbnail_regex_pattern') ? $this->t('Set') : $this->t('None')]);
    $summary[] = $this->t('Entity link: @state',
                          ['@state' => $this->getSetting('tripal_image_disable_link_to_entity') ? $this->t('Off') : $this->t('On')]);
    $summary[] = $this->t('Items per page: @ipp',
                          ['@ipp' => $this->getSetting('tripal_image_items_per_page')]);
    return $summary;
  }

}
