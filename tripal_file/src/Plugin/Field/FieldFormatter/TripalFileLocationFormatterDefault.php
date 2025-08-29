<?php

namespace Drupal\tripal_file\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\Attribute\FieldType;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\file\Entity\File;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * The default formatter for file content type.
 *
 * The default is a table to match the Tripal 3 version of this module.
 */
#[TripalFieldFormatter(
  id: 'tripal_file_location_formatter_default',
  label: new TranslatableMarkup('Tripal file location formatter'),
  description: new TranslatableMarkup('A tripal file location formatter'),
  field_types: [
    'tripal_file_location_type_default',
  ],
)]
class TripalFileLocationFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $rows = [];
    $header = [
      $this->t('Download File Name'),
      $this->t('Available at'),
      $this->t('Size'),
      $this->t('MD5'),
    ];
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');

    foreach ($items as $delta => $item) {
      $values = [
        'uri' => $item->get('fileloc_uri')->getString(),
        'rank' => $item->get('fileloc_rank')->getString(),
        'size' => $item->get('fileloc_size')->getString(),
        'md5checksum' => $item->get('fileloc_md5checksum')->getString(),
        'filename' => $item->get('fileloc_filename')->getString(),
      ];
      if (!$values['uri']) {
        return $elements;
      }
      $uri = $values['uri'];
      if (!$values['filename']) {
        $values['filename'] = $uri;
      }

      // The scheme will be NULL for an invalid URI.
      $scheme = parse_url($uri, PHP_URL_SCHEME);
      $host = '';
      $link = $uri;
      if ($scheme) {
        if ($scheme == 'public') {
          $host = \Drupal::config('system.site')->get('name');
          $url = \Drupal::service('file_url_generator')->generate($uri);
        }
        else {
          $host = parse_url($uri, PHP_URL_HOST);
          $url = Url::fromUri($uri);
        }
        $link = Link::fromTextAndUrl($values['filename'], $url);
      }

      $row = [$link, $host, $values['size'], $values['md5checksum']];
      $rows[$delta] = $row;
    }

    $elements[0] = [
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
//      '#attributes' => ['class' => 'responsive-enabled'],  // no change
      '#wrapper_attributes' => ['class' => 'container'],
    ];

    return $elements;
  }

}
