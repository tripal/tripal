<?php

namespace Drupal\tripal_image\Plugin\Field\FieldFormatter;

use Drupal\Component\Render\FormattableMarkup;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Markup;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\image\Entity\ImageStyle;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

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

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['token_string'] = '<strong>[name]</strong>: [value]';
    $settings['tripal_image_max_thumbnail_height'] = '200px';
    $settings['tripal_image_thumbnail_regex_pattern'] = '';
    $settings['tripal_image_thumbnail_regex_replacement'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    $list = [];
    $token_string = $this->getSetting('token_string');
    $max_thumbnail_height = $this->getSetting('tripal_image_max_thumbnail_height') ?: '200px';
    $regex_pattern = $this->getSetting('tripal_image_thumbnail_regex_pattern');
    $regex_replacement = $this->getSetting('tripal_image_thumbnail_regex_replacement');
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');
    $url_generator = \Drupal::service('file_url_generator');

    // Attach css for this field.
    $elements['#attached']['library'][] = 'tripal_image/image_formatter';

    foreach ($items as $delta => $item) {
      $eimage_properties = $item->get('eimage_properties')->getString();
      if ($eimage_properties) {
        $eimage_properties = json_decode($eimage_properties, TRUE);
      }
      $values = [
        'entity_id' => $item->get('entity_id')->getString(),
        'linker_type_id' => $item->get('linker_type_id')->getString(),
        'linker_type_name' => $item->get('linker_type_name')->getString(),
        'linker_rank' => $item->get('linker_rank')->getString(),
        'eimage_data' => $item->get('eimage_data')->getString(),
        'eimage_type' => $item->get('eimage_type')->getString(),
        'image_uri' => $item->get('image_uri')->getString(),
        'eimage_properties' => $eimage_properties,
      ];

      // The assumption is that images are either stored directly in the
      // eimage table as uuencoded data, or are linked using the image_uri
      // column, but not both at the same time.
      $image_markup = '';
      if ($values['image_uri']) {
        $url = $url_generator->generateAbsoluteString($values['image_uri']);
        $thumbnail_url = $url;
        if ($regex_pattern) {
          $thumbnail_url = preg_replace('/' . $regex_pattern . '/', $regex_replacement, $thumbnail_url);
        }
        $basename = basename($url);
        $image_markup = '<a href="' . $url . '"><img src="' . $thumbnail_url . '" alt="' . $basename . '"></a>';
      }
      elseif ($values['eimage_data']) {
        $binary_data = convert_uudecode($values['eimage_data']);
        $base64data = base64_encode($binary_data);
        $mime_type = 'image/' . $values['eimage_type'];
        $img_src = 'data:' . $mime_type . ';base64,' . $base64data;
        $image_markup = Markup::create('<img src="' . $img_src . '" alt="Decoded UUencoded Image">');
      }

      // Properties are converted to a list.
      // We only have three possible tokens.
      $property_list = [];
      if ($eimage_properties) {
        foreach ($eimage_properties as $label => $items) {
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
      // diplaying them, otherwise the image takes up the full width.
      // Link to the corresponding entity when one exists is not implemented
      // because eimage is not a content type.
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
    if (count($list) == 1) {
      $elements = $list;
    }

    // If more than one value has been found, display all values in an
    // unordered list.
    elseif (count($list) > 1) {
      $elements[0] = [
        '#type' => 'container',
        '#attributes' => [
          'class' => ['image-list'],
        ],
        'list' => [
          '#theme' => 'item_list',
          '#list_type' => 'ul',
          '#items' => $list,
          '#attributes' => [
            'class' => ['image-item'],
          ],
        ],
      ];
    }

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);

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
    return $summary;
  }

}
