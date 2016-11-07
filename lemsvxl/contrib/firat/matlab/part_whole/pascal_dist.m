function D = pascal_dist(bb1, bb2)
    area1 = (bb1(3) - bb1(1)) * (bb1(4) - bb1(2));
    area2 = (bb2(3) - bb2(1)) * (bb2(4) - bb2(2));
    com_bb = [bb1; bb2];
    minbb = min(com_bb);
    maxbb = max(com_bb);
    inter_bb = zeros(1,4);
    inter_bb(1) = maxbb(1);
    inter_bb(2) = maxbb(2);
    inter_bb(3) = minbb(3);
    inter_bb(4) = minbb(4);
    inter_area = (inter_bb(3) - inter_bb(1)) * (inter_bb(4) - inter_bb(2));
    union_area = area1 + area2 - inter_area;
    D = inter_area / union_area;
end
