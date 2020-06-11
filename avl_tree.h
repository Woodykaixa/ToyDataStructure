namespace cym {
    template <typename T>
    class avl_tree {

      public:
        enum class iteration_direction { Left, Right };

        class avl_node {

          private:
            T _value;
            avl_node* _left;
            avl_node* _right;
            int _height;
            int _balance_factor;
            friend class avl_tree;

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

            /**
             * Append param `node` as new subtree. Use param `dir` to specify
             * append left or right. Return the old subtree.
             *
             * @param node the new child to append
             * @param dir the direction to append child
             * @return the old child
             */
            avl_node* append(avl_node* node, iteration_direction& dir) {
                if (dir == iteration_direction::Left) {
                    return append_left(node);
                }
                return append_right(node);
            }

            avl_node* append(avl_node* node, iteration_direction&& dir) {
                if (dir == iteration_direction::Left) {
                    return append_left(node);
                }
                return append_right(node);
            }

            int height() const { return _height; }
            int balance_factor() const { return _balance_factor; }
        };

        avl_tree() { _root = nullptr; }

        /**
         * Insert a T-type value into AVL Tree. If param `e` is already in AVL
         * Tree, this function do nothing, otherwise it will insert `e` to
         * proper location just like Binary Search Tree, then it will call
         * `rebalance()`.
         *
         * @param e The value to insert.
         */
        void insert(T e) {
            avl_node* node = new avl_node(e);
            if (_root == nullptr) {
                _root = node;
                return;
            }

            avl_node* tmp = _root;
            dir_t dir;
            avl_node* pre = nullptr;
            while (tmp != nullptr) {
                if (tmp->_value == e) {
                    return;
                }
                pre = tmp;
                if (tmp->_value < e) {
                    dir = dir_t::Right;
                    tmp = tmp->_right;
                } else {
                    dir = dir_t::Left;
                    tmp = tmp->_left;
                }
            }
            pre->append(node, dir);

            rebalance(); // N/A
        }

        size_t height() const {
            return _root == nullptr ? 0 : _root->height() + 1;
        }

      private:
        using dir_t = iteration_direction;
        avl_node* _root;
        void rebalance() {}
    };

} // namespace cym