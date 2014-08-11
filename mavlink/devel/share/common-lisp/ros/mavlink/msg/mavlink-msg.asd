
(cl:in-package :asdf)

(defsystem "mavlink-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :std_msgs-msg
)
  :components ((:file "_package")
    (:file "Mavlink" :depends-on ("_package_Mavlink"))
    (:file "_package_Mavlink" :depends-on ("_package"))
  ))