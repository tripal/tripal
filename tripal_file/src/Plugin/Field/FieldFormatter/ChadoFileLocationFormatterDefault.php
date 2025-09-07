<?php

namespace Drupal\tripal_file\Plugin\Field\FieldFormatter;

use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * The default formatter for chado file location content type.
 *
 * The default is a table to match the Tripal 3 version of this module.
 */
#[TripalFieldFormatter(
  id: 'chado_file_location_formatter_default',
  label: new TranslatableMarkup('Chado file location formatter'),
  description: new TranslatableMarkup('A chado file location formatter'),
  field_types: [
    'chado_file_location_type_default',
  ],
)]
class ChadoFileLocationFormatterDefault extends ChadoFormatterBase {

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
    $elements['#attached']['library'][] = 'tripal_file/tripal_file.field.ChadoFileLocationFormatterDefault';
    $rows = [];
    $header = [
      $this->t('Download File Name'),
      $this->t('Available at'),
      $this->t('Size'),
      $this->t('MD5'),
    ];

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

      // Create a link to the file source as indicated by the URI.
      // The scheme here will be NULL for an invalid URI.
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
      '#attributes' => ['class' => 'chado-file-location-table'],
    ];

    return $elements;
  }

}
