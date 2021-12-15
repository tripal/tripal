<?php



interface TripalFieldItemInterface extends FieldItemInterface {
    public function tripalStorageId();
    public function tripalTypes($entityTypeId);
    public function tripalValuesTemplate();
    public function tripalLoad($properties,$entity);
    public function tripalSave($properties,$entity);
    public function tripalClear($entity);
}
