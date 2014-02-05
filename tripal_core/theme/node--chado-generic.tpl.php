<?php

if($teaser) {
  print render($content);
}
else { 
  $node_type = $node->type; ?>
  
  <script type="text/javascript">
    (function ($) {
      Drupal.behaviors.<?php print $node_type?>Behavior = {
        attach: function (context, settings){ 
          // hide all but the first data block 
          $(".tripal-data-block").hide().filter(":first-child").show();
     
          // when a title in the table of contents is clicked, then 
          // show the corresponding item in the details box 
          $(".tripal_toc_list_item_link").click(function(){
            var id = $(this).attr('id') + "-tripal-data-block";
            $(".tripal-data-block").hide().filter("#"+ id).fadeIn('fast');
            return false;
          }); 
    
          // if a ?block= is specified in the URL then we want to show the
          // requested block
          var block = window.location.href.match(/[\?|\&]block=(.+?)\&/)
          if(block == null){
            block = window.location.href.match(/[\?|\&]block=(.+)/)
          }
          if(block != null){
            $(".tripal-data-block").hide().filter("#" + block[1] + "-tripal-data-block").show();
          }
          // remove the 'active' class from the links section, as it doesn't
          // make sense for this layout
          $("a.active").removeClass('active');
        }
      };
    })(jQuery);
  </script>
  
  <div id="tripal_<?php print $node_type?>_contents" class="tripal-contents">
    <table id ="tripal-<?php print $node_type?>-contents-table" class="tripal-contents-table">
      <tr class="tripal-contents-table-tr">
        <td nowrap class="tripal-contents-table-td tripal-contents-table-td-toc"  align="left"><?php
        
          // print the table of contents. It's found in the content array 
          print $content['tripal_toc']['#markup'];
          
          // we may want to add the links portion of the contents to the sidebar
          //print render($content['links']);
          
          // remove the table of contents and links so thye doent show up in the 
          // data section when the rest of the $content array is rendered
          unset($content['tripal_toc']);
          unset($content['links']); ?>

        </td>
        <td class="tripal-contents-table-td-data" align="left" width="100%"> <?php
         
          // print the rendered content 
          print render($content); ?>
        </td>
      </tr>
    </table>
  </div> <?php 
}


