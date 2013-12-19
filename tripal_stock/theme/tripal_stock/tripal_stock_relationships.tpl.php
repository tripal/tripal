<?php
/* Typically in a Tripal template, the data needed is retrieved using a call to
 * tripal_core_expand_chado_vars function.  For example, to retrieve all 
 * of the stock relationships for this node, the following function call would be made:
 * 
 *   $stock = tripal_core_expand_chado_vars($stock,'table','stock_relationship');
 * 
 * However, this function call can be extremely slow when there are numerous relationships.
 * This is because the tripal_core_expand_chado_vars function is recursive and expands 
 * all data following the foreign key relationships tree.  Therefore, to speed retrieval
 * of data, a special variable is provided to this template:
 * 
 *   $stock->all_relationships;
 *   
 * This variable is an array with two sub arrays with the keys 'object' and 'subject'.  The array with
 * key 'object' contains relationships where the stock is the object, and the array with
 * the key 'subject' contains relationships where the stock is the subject
 */

$stock = $variables['node']->stock;

$all_relationships = $stock->all_relationships;
$object_rels = $all_relationships['object'];
$subject_rels = $all_relationships['subject'];

// make the stock type a bit more human readable
$stock_type =  preg_replace("/_/", ' ', $stock->type_id->name);

if (count($object_rels) > 0 or count($subject_rels) > 0) {
?>
  <div id="tripal_stock-relationships-box" class="tripal_stock-info-box tripal-info-box">
    <div class="tripal_stock-info-box-title tripal-info-box-title">Relationships</div>
    <!--  <div class="tripal_stock-info-box-desc tripal-info-box-desc"></div> --><?php
    
      // first add in the subject relationships.  
      foreach ($subject_rels as $rel_type => $rels){
         // make the type a bit more human readable
         $rel_type = preg_replace("/_/", ' ', $rel_type);
         $rel_type = preg_replace("/^is/", '', $rel_type);
         // iterate through each parent   
         foreach ($rels as $obj_type => $objects){?>
           <p>This stock is a <b><?php print $rel_type ?></b> of the following <?php print $obj_type ?> stock(s):
           <table id="tripal_stock-relationships_as_object-table" class="tripal_stock-table tripal-table tripal-table-horz">
             <tr>
               <th>Stock Name</th>
               <th>Type</th>
               <th>Description</th>
             </tr> <?php
             foreach ($objects as $object){ ?>
               <tr>
                 <td><?php 
                    if ($object->nid) {
                      print "<a href=\"" . url("node/" . $object->nid) . "\" target=\"_blank\">" . $object->name . "</a>";
                    }
                    else {
                      print $object->name;
                    } ?>
                 </td>
                 <td><?php print ucwords(preg_replace('/_/', ' ', $object->obj_type)) ?></td> 
                 <td><?php print $object->value ?></td>                
               </tr> <?php
             } ?>
             </table>
             </p><br><?php
         }
      }
      
      // second add in the object relationships.  
      foreach ($object_rels as $rel_type => $rels){
         // make the type more human readable
         $rel_type = preg_replace('/_/', ' ', $rel_type);
         $rel_type = preg_replace("/^is/", '', $rel_type);
         // iterate through the children         
         foreach ($rels as $subject_type => $subjects){?>
           <p>The following <?php print $subject_type ?> stocks are a <b><?php print $rel_type ?></b> of this stock:
           <table id="tripal_stock-relationships_as_object-table" class="tripal_stock-table tripal-table tripal-table-horz">
             <tr>
               <th>Stock Name</th>
               <th>Type</th>
               <th>Description</th>
             </tr> <?php
             foreach ($subjects as $subject){ ?>
               <tr>
                 <td><?php 
                    if ($subject->nid) {
                      print "<a href=\"" . url("node/" . $subject->nid) . "\" target=\"_blank\">" . $subject->name . "</a>";
                    }
                    else {
                      print $subject->name;
                    } ?>
                 </td>
                 <td><?php print ucwords(preg_replace('/_/', ' ', $subject->sub_type)) ?></td>   
                 <td><?php print $subject->value ?></td>              
               </tr> <?php
             } ?>
             </table>
             </p><br><?php
         }
      } ?>
  </div> <?php
}
