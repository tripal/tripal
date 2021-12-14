<?php



interface TripalFieldItemInterface extends FieldItemInterface {
    public function tripalStorageId();
    public function tripalTypes($entityTypeId);
    public function tripalValues($entityTypeId,$entityId);
    public function tripalLoad($properties,$entity);
    public function tripalSave($properties,$entity);
    public function tripalClear($entity);
}
