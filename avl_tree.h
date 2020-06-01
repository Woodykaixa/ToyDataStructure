namespace cym {
    template <typename T>
    class avl_tree {

      public:
        class avl_node {

          private:
            T _value;
            avl_node* _left;
            avl_node* _right;
            int _height;
            int _balance_factor;

          public:
            avl_node(T v)
                : _value(v), _left(nullptr), _right(nullptr), _height(0),
                  _balance_factor(0) {}

            bool need_rotation() {
                return _balance_factor < -1 || 1 < _balance_factor;
            }

          private:
            static int get_height(avl_node* node) {
                if (node == nullptr) {
                    return 0;
                }
                return node->_height + 1;
            }

            void recalculate_height_and_factor() {
                int left_height = get_height(_left);
                int right_height = get_height(_right);
                _balance_factor = right_height - left_height;
                _height =
                    left_height > right_height ? left_height : right_height;
            }

          public:
            /**
             * Append param `node` as new left subtree. Return the old left
             * subtree.
             *
             * @param node the new left child to append
             * @return the old left child
             */
            avl_node* append_left(avl_node* node) {
                avl_node* old = _left;
                _left = node;
                recalculate_height_and_factor();
                return old;
            }

            /**
             * Append param `node` as new right subtree. Return the old right
             * subtree.
             *
             * @param node the new right child to append
             * @return the old left child
             */
            avl_node* append_right(avl_node* node) {
                avl_node* old = _right;
                _right = node;
                recalculate_height_and_factor();
                return old;
            }

            int height() const { return _height; }
            int balance_factor() const { return _balance_factor; }
        };

        avl_tree() { _root = nullptr; }


      private:
        avl_node* _root;
    };

} // namespace cym