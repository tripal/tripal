<?php
/* Typically in a Tripal template, the data needed is retrieved using a call to
 * tripal_core_expand_chado_vars function.  For example, to retrieve all 
 * of the feature relationships for this node, the following function call would be made:
 * 
 *   $feature = tripal_core_expand_chado_vars($feature,'table','feature_relationship');
 * 
 * However, this function call can be extremely slow when there are numerous relationships.
 * This is because the tripal_core_expand_chado_vars function is recursive and expands 
 * all data following the foreign key relationships tree.  Therefore, to speed retrieval
 * of data, a special variable is provided to this template:
 * 
 *   $feature->all_relationships;
 *   
 * This variable is an array with two sub arrays with the keys 'object' and 'subject'.  The array with
 * key 'object' contains relationships where the feature is the object, and the array with
 * the key 'subject' contains relationships where the feature is the subject
 */
$feature = $variables['node']->feature;

$all_relationships = $feature->all_relationships;
$object_rels = $all_relationships['object'];
$subject_rels = $all_relationships['subject'];

if (count($object_rels) > 0 or count($subject_rels) > 0) {
?>
  <div id="tripal_feature-relationships-box" class="tripal_feature-info-box tripal-info-box">
    <div class="tripal_feature-info-box-title tripal-info-box-title">Relationships</div>
    <div class="tripal_feature-info-box-desc tripal-info-box-desc"></div> <?php
    
      // first add in the subject relationships.  
      foreach ($subject_rels as $rel_type => $rels){
         foreach ($rels as $obj_type => $objects){?>           
           <p>This <?php print $feature->type_id->name;?> is <?php print $rel_type ?> the following <b><?php print $obj_type ?></b> feature(s):
           <table id="tripal_feature-relationships_as_object-table" class="tripal_feature-table tripal-table tripal-table-horz">
             <tr>
               <th>Feature Name</th>
               <th>Unique Name</th>
               <th>Species</th>
               <th>Type</th>
             </tr> <?php
             foreach ($objects as $object){ ?>
               <tr>
                 <td><?php 
                    if ($object->record->nid) {
                      print "<a href=\"" . url("node/" . $object->record->nid) . "\" target=\"_blank\">" . $object->record->object_id->name . "</a>";
                    }
                    else {
                      print $object->record->object_id->name;
                    } ?>
                 </td>
                 <td><?php print $object->record->object_id->uniquename ?></td>
                 <td><?php print $object->record->object_id->organism_id->genus . " " . $object->record->object_id->organism_id->species; ?></td>
                 <td><?php print $object->record->object_id->type_id->name ?></td>                 
               </tr> <?php
             } ?>
             </table>
             </p><br><?php
         }
      }
      
      // second add in the object relationships.  
      foreach ($object_rels as $rel_type => $rels){
         foreach ($rels as $subject_type => $subjects){?>
           <p>The following <b><?php print $subjects[0]->record->subject_id->type_id->name ?></b> feature(s) are <?php print $rel_type ?> this <?php print $feature->type_id->name;?>:
           <table id="tripal_feature-relationships_as_object-table" class="tripal_feature-table tripal-table tripal-table-horz">
             <tr>
               <th>Feature Name</th>
               <th>Unique Name</th>
               <th>Species</th>
               <th>Type</th>
             </tr> <?php
             foreach ($subjects as $subject){ ?>
               <tr>
                 <td><?php 
                    if ($subject->record->nid) {
                      print "<a href=\"" . url("node/" . $subject->record->nid) . "\" target=\"_blank\">" . $subject->record->subject_id->name . "</a>";
                    }
                    else {
                      print $subject->record->subject_id->name;
                    } ?>
                 </td>
                 <td><?php print $subject->record->subject_id->uniquename ?></td>
                 <td><?php print $subject->record->subject_id->organism_id->genus . " " . $subject->record->subject_id->organism_id->species; ?></td>
                 <td><?php print $subject->record->subject_id->type_id->name ?></td>                 
               </tr> <?php
             } ?>
             </table>
             </p><br><?php
         }
      }
    ?>
  </div> <?php
}
