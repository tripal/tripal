<?php
class chado_views_handler_field_chado_base__organism_id extends views_handler_field {

  function render($values) {
    $value = $this->get_value($values);

    return $value['label'];
  }
}
