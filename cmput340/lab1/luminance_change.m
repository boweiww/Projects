function  J = luminance_change(I, target, value)
    if target=='c'
       J=I*value;
    else if target=='b'
       J=I+value;
    end
end