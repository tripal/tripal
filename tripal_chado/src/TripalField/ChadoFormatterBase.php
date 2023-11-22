<?php

namespace Drupal\tripal_chado\TripalField;

use Drupal\Core\Field\FormatterBase;
use Drupal\tripal\TripalField\TripalFormatterBase;

/**
 * Defines the Chado field formatter base class.
 */
abstract class ChadoFormatterBase extends TripalFormatterBase {

  /**
   * Form element validation handler for token strings
   *
   * @param array $form
   *   The form where the settings form is being included in.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) form.
   */
  public static function settingsFormValidateTokenString(array $form, FormStateInterface $form_state) {
    $plugin_definition = $this->getPluginDefinition();
    $id = $plugin_definition['id'];
    if (!isset($plugin_definition['valid_tokens'])) {
      return;
    }
    $valid_tokens = $plugin_definition['valid_tokens'];
    $valid_tokens_str = '"' . implode('", "', $valid_tokens) . '"';

    // This form state contains settings for all of the fields for the
    // current content type, we only validate our own field.
    $field_values = $form_state->getValue('fields');
    foreach ($field_values as $field => $field_settings) {
      if (($field_settings['type'] == $id)
          and (array_key_exists('settings_edit_form', $field_settings))) {
        $token_string = $field_settings['settings_edit_form']['settings']['token_string'];
        if ($token_string) {
          $n_tokens = 0;
          $n_invalid = 0;
          // Extract a list of tokens, we need at least one.
          preg_match_all('/\[[^\[\]]*\]/', $token_string, $matches);
          foreach ($matches[0] as $index => $match) {
            if (in_array($match, $valid_tokens)) {
              $n_tokens++;
            }
            else {
              $n_invalid++;
            }
          }

          // Return validation error to the user with offending field highlighted
          if ($n_invalid) {
            $form_state->setErrorByName('fields][' . $field . '][settings_edit_form][settings][token_string',
                'The token string contains an invalid token, valid tokens are: ' . $valid_tokens_str);
          }
          elseif (!$n_tokens) {
            $form_state->setErrorByName('fields][' . $field . '][settings_edit_form][settings][token_string',
                'The token string must contain at least one of: ' . $valid_tokens_str);
          }
        }
      }
    }
  }

}
