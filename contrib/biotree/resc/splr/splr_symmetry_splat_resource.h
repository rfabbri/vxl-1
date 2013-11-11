#if !defined(SPLR_SYMMETRY_SPLAT_RESOURCE_H_)
#define SPLR_SYMMETRY_SPLAT_RESOURCE_H_

template <class T>
class splr_symmetry_splat_resource : public splr_splat_resource<T> {
 private:
  splr_symmetry * symmetry_;   
  virtual void initialize_symmetry(const xscan_scan &scan) = 0;
 public:
    void set_symmetry(splr_symmetry * symmetry);
    void initialize(const xscan_scan &scan,
      const xmvg_composite_filter_3d<T> &filter_3d);
    ~splr_symmetry_splat_resource();
};

#endif
