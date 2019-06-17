/**
 *  TripalUploadFile Object
 */
(function($) {

  "use strict";

  /**
   * The constructor function.
   */
  var TripalUploadFile = function (file, options) {
   
    this.file = file;
    this.options = options;
    this.file_size = file.size;
    this.chunk_size = (1024 * 2000); 
    this.total_chunks = ((this.file.size % this.chunk_size == 0) ? Math.floor(this.file.size / this.chunk_size) : Math.floor(this.file.size / this.chunk_size) + 1); 
    this.curr_chunk = 0;
    this.status = 'pending';
    this.file_id = null;
   
    if ('mozSlice' in file) {
      this.slice_method = 'mozSlice';
    }
    else if ('webkitSlice' in file) {
      this.slice_method = 'webkitSlice';
    }
    else {
      this.slice_method = 'slice';
    }

    var self = this;
    this.xhr = new XMLHttpRequest();
    this.xhr.onload = function() {
      self._onChunkComplete();
    }

    // Respond to changes in connection
    if ('onLine' in navigator) {
      window.addEventListener('online', function () {self._onConnectionFound});
      window.addEventListener('offline', function () {self._onConnectionLost});
    }
  

    // ------------------------------------------------------------------------
    // Internal Methods 
    // ------------------------------------------------------------------------
    /**
     * 
     */
    this._upload = function() {
      // Cacluate the range for the current chunk
      var range_start = this.curr_chunk * this.chunk_size;
      var range_end = range_start + this.chunk_size;
      
      // If we've gone beyond the number of chunks then just quit.
      if (this.curr_chunk > this.total_chunks) {
        this._onChunkComplete();
        return;
      }

      // Prevent range overflow
      if (this.range_end > this.file_size) {
        this.range_end = this.file_size;
      }
         
      var chunk = this.file[this.slice_method](range_start, range_end);
      var url = this.options.url + '/' + this.file.name + '/save/' + this.curr_chunk;
      
      this.xhr.open('PUT', url, true);
      this.xhr.overrideMimeType('application/octet-stream');  
      this.xhr.setRequestHeader('Content-Range', 'bytes ' + range_start + '-' + range_end + '/' + this.file_size);
      
      this.xhr.send(chunk);
    };
    
    /**
     * Converts a file size into a human readable value.
     * Borrowed function from:
     * http://stackoverflow.com/questions/10420352/converting-file-size-in-bytes-to-human-readable
     */
    this._getReadableSize = function(bytes, si) {
        var thresh = si ? 1000 : 1024;
        
        if(Math.abs(bytes) < thresh) {
            return bytes + ' B';
        }
        var units = si
          ? ['kB','MB','GB','TB','PB','EB','ZB','YB']
          : ['KiB','MiB','GiB','TiB','PiB','EiB','ZiB','YiB'];
        var u = -1;
        do {
          bytes /= thresh;
          ++u;
        } 
        while(Math.abs(bytes) >= thresh && u < units.length - 1);
        return bytes.toFixed(1) + ' ' + units[u];
    };
    
    /**
     * Queries server to see what chunk the loading left off at.
     */
    this._checkUpload = function() {
      var url = this.options.url + '/' + this.file.name + '/check/';
      var self = this;
      $.ajax({
        url : url,
        data : {
          'module' : this.options['module'],
          'chunk_size' : this.chunk_size,
          'file_size' : this.file_size,
        },
        success : function(data, textStatus, jqXHR) {
          if (data['status'] == 'failed') {
            self.status = 'failed';
            self.updateStatus();
            alert(data['message']);
          }
          else {
            self.curr_chunk =  data['curr_chunk'];
            self.status = 'uploading';
            self._upload();
            self.updateStatus();
            self.updateProgressBar();
          }
        },
        error : function(jqXHR, textStatus, errorThrown) {
          alert(errorThrown);
          self.curr_chunk = 0;
          self._upload();
        }
      });
    }
    
    /**
     * 
     */
    this._mergeChunks = function() {
        var url = this.options.url + '/' + this.file.name + '/merge';
        var self = this;
        $.ajax({
          url : url,
          data : {
            'module' : this.options['module'],
            'file_size' : this.file_size,
          },
          success : function(data, textStatus, jqXHR) {
            if (data['status'] == 'completed') {
              self.file_id = data['file_id'];
              self.status = 'completed';
              self.updateStatus();
            }
            else {
              self.status = 'failed';
              self.updateStatus();
              alert(data['message']);
            }
          },
          error : function() {
            self.status = 'failed';
            self.updateStatus();
          }
        });
    }
   
    // ------------------------------------------------------------------------
    // Event Handlers
    // ------------------------------------------------------------------------
   
    this._onChunkComplete = function() {
      // If the curr_chunk and the total_chunks is the same then
      // we've reached the end.
      if (this.curr_chunk >= this.total_chunks) {
        this.updateStatus();
        this._onUploadComplete();

        return;
      }

      // Continue as long as we aren't paused
      if (this.status == 'uploading') {
        this._upload();
        this.curr_chunk++;
        this.updateProgressBar();
      }
    };
    /**
     * 
     */
    this._onUploadComplete = function() {
      this.status = 'merging';
      this._mergeChunks();
      this.updateStatus();
    };
    /**
     * When a connection has been lost but reistablished then resume uploads.
     */
    this._onConnectionFound = function() {
      this.resume();
    };
 
    /**
     * When a cnnection has been lost then pause uploads.
     */
    this._onConnectionLost = function() {
      this.pause();
    };
   
    // ------------------------------------------------------------------------
    // Public Methods 
    // ------------------------------------------------------------------------
    /**
     * 
     */
    this.getProgressBar = function() {
      var progress_id = this.options['progress'];
      return '<div id="' + progress_id + '" class="tripal-uploader-progress-label">0%</div>';
    };
    
    /**
     * 
     */
    this.getLinks = function() {
      var links_id = this.options['links'];
      return '<div id="' + links_id + '" class="tripal-uploader-links">0%</div>';
    }
    
    this.getCategory = function() {
      return this.options['category'];
    }
    this.getIndex = function() {
      return this.options['category'];
    }
    this.getTName = function() {
      return this.options['tname'];
    }
    this.getFileName = function() {
      return this.file.name;
    }
    /**
     * 
     */
    this.getFileSize = function(readable) {
      if (readable) {
        return this._getReadableSize(this.file.size, true);
      }
      else {
        return this.file.size;
      }
    };
    /**
     * Updates the links, status text and status bar.
     */
    this.updateStatus = function() {

      var progress_id = this.options['progress'];
      
      // Add the progress text.
      $('#' + progress_id).html('');
      if (this.status == 'cancelled') {
        $("<span>", {
          'text' : 'Cancelled',
        }).appendTo('#' + progress_id)
      }
      else if (this.status == 'checking') {
        $("<span>", {
          'text' : 'Checking...',
        }).appendTo('#' + progress_id)
      }
      else if (this.status == 'merging') {
        $("<span>", {
          'text' : 'Processing...',
        }).appendTo('#' + progress_id)
      }
      else if (this.status == 'failed') {
        $("<span>", {
          'text' : 'Failed',
        }).appendTo('#' + progress_id)
      }
      else if (this.status == 'completed') {
        $("<span>", {
          'text' : 'Complete',
        }).appendTo('#' + progress_id)
        // Set the parent's target field.
        var parent = self.options['parent'];
        var tname = self.options['tname'];
        var category = self.options['category'];
        parent.setTarget(tname);
      }
      else if (this.status == 'paused') {
        $("<span>", {
          'text' : 'Paused',
        }).appendTo('#' + progress_id)
      }
      
      // Add a throbber if the status is uploading
      if (this.status == 'uploading' || this.status == 'checking' || this.status == 'merging') {
        $("<img>", {
           'src': tripal_path + '/theme/images/ajax-loader.gif',
           'class' : 'tripal-uploader-chunked-file-progress-throbber',
         }).appendTo('#' + progress_id);
      }
      
      // Add the appropriate links.
      var links_id = this.options['links'];
      var category = this.options['category'];
      $('#' + links_id).html('');
      if (this.status == 'cancelled') {
        $("<a>", {
          'id': links_id + '-pending',
          'class': category + '-pending',
          'href': 'javascript:void(0);',
          'text': 'Restore',
        }).appendTo('#' + links_id);
        $('#' + links_id + '-pending').click(function() {
          self.pending();
        })
      }
      if (this.status == 'pending') {
        $("<a>", {
          'id': links_id + '-cancel',
          'class': category + '-cancel',
          'href': 'javascript:void(0);',
          'text': 'Cancel',
        }).appendTo('#' + links_id);
        $('#' + links_id + '-cancel').click(function() {
          self.cancel();
        })
      }
      if (this.status == 'uploading') {
        $("<a>", {
          'id': links_id + '-pause',
          'class': category + '-pause',
          'href': 'javascript:void(0);',
          'text': 'Pause',
        }).appendTo('#' + links_id);
        $('#' + links_id + '-pause').click(function() {
          self.pause();
        })
      }
      if (this.status == 'paused') {
        $("<a>", {
          'id': links_id + '-resume',
          'class': category + '-resume',
          'href': 'javascript:void(0);',
          'text': 'Resume',
        }).appendTo('#' + links_id);
        $('#' + links_id + '-resume').click(function() {
          self.resume();
        })
      }
        
      // Add the remove link.
      $("<a>", {
        'id': links_id + '-remove',
        'class': category + '-remove',
        'href': 'javascript:void(0);',
        'text': ' Remove',
      }).appendTo('#' + links_id);
      $('#' + links_id + '-remove').click(function() {
        var parent = self.options['parent'];
        var index = self.options['index'];
        var tname = self.options['tname'];
        var category = self.options['category'];
        parent.removeFile(tname, category, index);
        parent.updateTable(category);
        // Unset the parent's target field.
        parent.setTarget(tname);
        self.cancel();
      })
    }
    /**
     * Updates the status bar progress only.
     */
    this.updateProgressBar = function() {
      var progress_id = this.options['progress'];
      var progress = (this.curr_chunk / this.total_chunks) * 100;
      var self = this;

      // Calculate the amount of the file transferred.
      var size_transferred = this.curr_chunk * this.chunk_size;
      size_transferred = this._getReadableSize(size_transferred, true);
      
      if (this.status == 'uploading') {
        $('#' + progress_id).html('');
        $("<span>", {
          'class': 'tripal-uploader-chunked-file-progress-label',
          'text': size_transferred,
        }).appendTo($("<div>", {
          'id': progress_id + '-bar',
          'class': 'tripal-uploader-chunked-file-progress',
          'width': progress + '%'
        }).appendTo($("<div>", {
          'id': progress_id + '-box',
          'class': 'tripal-uploader-chunked-file-progress',
        }).appendTo('#' + progress_id)));

      }
      if (this.status == 'uploading' || this.status == 'checking' || this.status == 'merging') {
        $("<img>", {
           'src': tripal_path + '/theme/images/ajax-loader.gif',
           'class' : 'tripal-uploader-chunked-file-progress-throbber',
         }).appendTo('#' + progress_id);
      }
      
    };
    /**
     * 
     */
    this.cancel = function() {
      this.status = 'cancelled';
      this.updateStatus();
    }
    /**
     * 
     */
    this.pending = function() {
      this.status = 'pending';
      this.updateStatus();
    }
    /**
     * 
     */
    this.start = function() {
      if (this.status == 'pending') {
        // Change the status to checking. The first thing we'll
        // do is see what's already present on the server.
        this.status = 'checking';
        this.curr_chunk = this._checkUpload();
      }
    };

    /**
     * 
     */
    this.pause = function() {
      this.status = 'paused';
      this.updateStatus();
    };
    /**
     * 
     */
    this.resume = function() {
      this.status = 'uploading';
      this.updateStatus();
      this.updateProgressBar();
      this._upload();
    };
  };
  
  // Export the objects to the window for use in other JS files.
  window.TripalUploadFile = TripalUploadFile;
  
})(jQuery);
