function Y_new=polynomial_interp(X,Y,X_new,n)

    [m_x,~] = size( X );
    [ m_x_new, ~ ] = size( X_new );
    A = ones( m_x, n + 1 );
    A_new = ones( m_x_new, n + 1 );
    %   Using for to generalize n degrees polynomial.
    for i = 1:1:n+1
        A(:,i) = X.^(i-1);
        A_new(:,i) = X_new.^(i - 1);
    end
    coe = A\Y;
    Y_new = A_new * coe;
    %p = polyfit(X,Y,n);
    %Y_new = polyval(p,X_new);

end
